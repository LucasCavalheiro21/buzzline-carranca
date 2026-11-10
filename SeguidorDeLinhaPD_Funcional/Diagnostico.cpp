#include "Diagnostico.h"
#include "Configuracoes.h"
#include "Sensores.h"
#include "Motores.h"
#include "Sinalizacao.h"

namespace {
  // Aceita tanto o botao D12 (INPUT_PULLUP) quanto a letra pelo Serial.
  // Para o botao, exige SOLTAR -> PRESSIONAR -> SOLTAR com antirruido;
  // um unico pressionamento nao avanca duas etapas de calibracao.
  void aguardarConfirmacao(uint8_t etapa, char tecla) {
    uint8_t fase = 0; // 0: solto, 1: pressionado, 2: solto novamente
    bool leituraAnterior = digitalRead(Config::BOTAO);
    unsigned long mudouEm = millis();

    while (true) {
      Sinalizacao::aguardandoSuperficie(etapa);

      if (Serial.available() > 0) {
        const char c = Serial.read();
        if (c == tecla || c == char(tecla - 'a' + 'A')) {
          Sinalizacao::apagar();
          return;
        }
      }

      const bool leitura = digitalRead(Config::BOTAO);
      const unsigned long agora = millis();
      if (leitura != leituraAnterior) {
        leituraAnterior = leitura;
        mudouEm = agora;
      }
      if (agora - mudouEm >= Config::DEBOUNCE_MS) {
        const bool solto = leitura == HIGH;
        if ((fase == 0 && solto) || (fase == 1 && !solto)) {
          ++fase;
          mudouEm = agora;
        } else if (fase == 2 && solto) {
          Sinalizacao::apagar();
          return;
        }
      }
      delay(5);
    }
  }

  void ordenar(uint16_t vetor[], uint8_t n) {
    for (uint8_t i = 1; i < n; ++i) {
      uint16_t atual = vetor[i];
      int8_t j = i - 1;
      while (j >= 0 && vetor[j] > atual) {
        vetor[j + 1] = vetor[j];
        --j;
      }
      vetor[j + 1] = atual;
    }
  }

  uint16_t mediaAparada(const uint16_t valores[], uint8_t n) {
    uint32_t soma = 0;
    for (uint8_t i = Config::DESCARTAR_EXTREMOS;
         i < n - Config::DESCARTAR_EXTREMOS; ++i) soma += valores[i];
    const uint8_t usados = n - 2 * Config::DESCARTAR_EXTREMOS;
    return static_cast<uint16_t>((soma + usados / 2) / usados);
  }

  void coletarReferencias(uint16_t medias[Config::NUM_SENSORES]) {
    uint16_t amostras[Config::NUM_SENSORES][Config::N_AMOSTRAS];
    uint16_t bruto[Config::NUM_SENSORES];

    Serial.println(F("Coletando 25 amostras em ~5 s. Mantenha a altura fixa."));
    for (uint8_t instante = 0; instante < Config::N_AMOSTRAS; ++instante) {
      Sinalizacao::coletando();
      Sensores::lerBrutos(bruto);
      for (uint8_t sensor = 0; sensor < Config::NUM_SENSORES; ++sensor)
        amostras[sensor][instante] = bruto[sensor];
      delay(Config::INTERVALO_AMOSTRAS_MS);
    }
    Sinalizacao::apagar();
    for (uint8_t sensor = 0; sensor < Config::NUM_SENSORES; ++sensor) {
      ordenar(amostras[sensor], Config::N_AMOSTRAS);
      medias[sensor] = mediaAparada(amostras[sensor], Config::N_AMOSTRAS);
    }
  }

  void mostrarReferencias(const __FlashStringHelper* titulo,
                          const uint16_t valores[Config::NUM_SENSORES]) {
    Serial.println(titulo);
    for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
      Serial.print(F("  S")); Serial.print(i + 1);
      Serial.print(F(" = ")); Serial.println(valores[i]);
    }
  }

  bool testarSensoresECalibrar() {
    uint16_t branco[Config::NUM_SENSORES];
    uint16_t preto[Config::NUM_SENSORES];

    Serial.println(F("[1/2] CALIBRACAO QTR-8RC"));
    Serial.println(F("BRANCO: LED 1 pulso. Posicione TODOS os sensores no branco."));
    Serial.println(F("Pressione e SOLTE D12 ou envie 'b' no Monitor Serial."));
    aguardarConfirmacao(1, 'b');
    coletarReferencias(branco);
    mostrarReferencias(F("Referencia de BRANCO (us):"), branco);

    Serial.println(F("PRETO: LED 2 pulsos. Posicione TODOS os sensores no preto."));
    Serial.println(F("Pressione e SOLTE D12 ou envie 'p' no Monitor Serial."));
    aguardarConfirmacao(2, 'p');
    coletarReferencias(preto);
    mostrarReferencias(F("Referencia de PRETO (us):"), preto);

    Serial.println(F("Contrastes (preto - branco), us:"));
    bool referenciasValidas = true;
    for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
      const int32_t contraste = int32_t(preto[i]) - int32_t(branco[i]);
      Serial.print(F("  S")); Serial.print(i + 1);
      Serial.print(F(" = ")); Serial.print(contraste);
      if (preto[i] < Config::PRETO_MINIMO_US) {
        Serial.print(F(" [FALHA: referencia preta abaixo de "));
        Serial.print(Config::PRETO_MINIMO_US);
        Serial.print(F(" us]"));
        referenciasValidas = false;
      }
      if (contraste < Config::CONTRASTE_MINIMO_US) {
        Serial.print(F(" [FALHA: contraste insuficiente]"));
        referenciasValidas = false;
      }
      Serial.println();
    }
    if (!referenciasValidas || !Sensores::calibrar(branco, preto)) {
      Serial.println(F("[FALHA] Calibracao invalida. Movimento bloqueado."));
      Serial.println(F("Confira S1 a S7 inteiros sobre preto, altura, alimentacao e luz."));
      Serial.println(F("Corrija a superficie e pressione RESET para repetir."));
      return false;
    }
    Serial.println(F("[OK] Sete sensores calibrados; botao testado nos comandos se usado."));
    return true;
  }

  bool conferirPinosDosMotores() {
    // Verificacao de configuracao, NAO teste de giro/corrente do motor.
    const uint8_t p[] = {Config::MA_IN1, Config::MA_IN2,
                         Config::MB_IN1, Config::MB_IN2};
    for (uint8_t i = 0; i < 4; ++i) {
      if (p[i] == 255 || p[i] == Config::BOTAO ||
          p[i] == Config::LED_STATUS) return false;
      for (uint8_t j = i + 1; j < 4; ++j)
        if (p[i] == p[j]) return false;
      for (uint8_t j = 0; j < Config::NUM_SENSORES; ++j)
        if (p[i] == Config::PINOS_SENSORES[j]) return false;
    }
    return true;
  }
}

bool Diagnostico::executar() {
  Motores::parar();
  Serial.println(F("===================================="));
  Serial.println(F("CALIBRACAO E DIAGNOSTICO SEM USB"));
  Serial.println(F("Serial opcional, em 115200 baud."));
  Serial.println(F("LED: 1 pulso branco; 2 preto; rapido OK; aceso FALHA."));
  Serial.println(F("===================================="));

  if (!conferirPinosDosMotores()) {
    Serial.println(F("[FALHA] Conflito ou pinagem invalida nos motores."));
    return false;
  }
  if (!testarSensoresECalibrar()) return false;

  Serial.println(F("[2/2] CONFIGURACAO DOS MOTORES"));
  Serial.println(F("[OK] Pinos sem conflito. A = DIREITO (5/6); B = ESQUERDO (10/9)."));
  Serial.println(F("[AVISO] Nao e possivel testar giro/corrente sem sensores extras."));
  if (!Config::HABILITAR_MOVIMENTACAO) {
    Serial.println(F("[TRAVA] Movimentacao desabilitada em Configuracoes.h."));
    Serial.println(F("[TRAVA] O LED de OK confirma apenas diagnosticos executados."));
  } else {
    Serial.println(F("[AVISO] Movimento habilitado: EXIGE teste previo das rodas suspensas."));
  }
  return true;
}
