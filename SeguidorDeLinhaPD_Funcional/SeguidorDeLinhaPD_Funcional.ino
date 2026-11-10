#include "Configuracoes.h"
#include "Sensores.h"
#include "Motores.h"
#include "Botao.h"
#include "ControlePD.h"
#include "Diagnostico.h"
#include "Sinalizacao.h"

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Sinalizacao::iniciar();
  Sensores::iniciar();
  Motores::iniciar();
  Botao::iniciar();
  ControlePD::reiniciar();

  if (!Diagnostico::executar()) {
    Motores::parar();
    Serial.println(F("DIAGNOSTICO REPROVADO. LED aceso; corrija e pressione RESET."));
    Sinalizacao::reprovado();
    while (true) {
      Motores::parar();
      delay(100);
    }
  }

  Sinalizacao::aprovado(); // 2 segundos de piscadas rapidas; depois LED apagado.
  Botao::iniciar(); // Reiniciar em OFF apos o diagnostico.
  Serial.println(F("DIAGNOSTICO APROVADO. LED piscou rapido por 2 segundos."));
  if (Motores::movimentacaoLiberada()) {
    Serial.println(F("PRONTO PARA RODAR. D12 liga/desliga."));
  } else {
    Serial.println(F("MOTORES BLOQUEADOS. D12 liga/desliga SIMULACAO PD."));
  }
}

void loop() {
  // Recuperação de curvas fechadas
  static int8_t ultimoLadoLinha = 0;
  static unsigned long instanteUltimoLado = 0;
  static unsigned long inicioRecuperacao = 0;
  static bool recuperandoLinha = false;

  // Controle da passagem por cruzamentos
  static bool cruzamentoAtivo = false;
  static bool linhaAnteriorValida = false;
  static unsigned long inicioCruzamento = 0;

  // Parâmetros iniciais para testes
  constexpr int ERRO_LATERAL_MINIMO = 350;
  constexpr int PWM_RECUPERACAO = 90;
  constexpr unsigned long TEMPO_RECUPERACAO_MS = 400;
  constexpr unsigned long MEMORIA_LADO_MS = 450;
  Botao::atualizar();

  if (!Botao::ligado()) {
    ControlePD::reiniciar();
    Motores::frear(); // HIGH/HIGH so quando os dois motores estiverem habilitados.
    // Limpar a memória da recuperação de linha
    recuperandoLinha = false;
    ultimoLadoLinha = 0;
    instanteUltimoLado = 0;
    inicioRecuperacao = 0;

    // Limpar a memória dos cruzamentos
    cruzamentoAtivo = false;
    linhaAnteriorValida = false;
    inicioCruzamento = 0;

    return;
  }

  Sensores::Leituras leitura = Sensores::ler();
  int pwmEsquerdo = 0;
  int pwmDireito = 0;
  ControlePD::Resultado pd = {};

  constexpr unsigned long TEMPO_CRUZAMENTO_MS = 250;

  if (leitura.faixaLarga) {
    recuperandoLinha = false;
    ultimoLadoLinha = 0;

    if (!cruzamentoAtivo && linhaAnteriorValida) {
      cruzamentoAtivo = true;
      inicioCruzamento = millis();
      ControlePD::reiniciar();
    }

    if (cruzamentoAtivo &&
        millis() - inicioCruzamento < TEMPO_CRUZAMENTO_MS) {

      // Mesma velocidade nas duas rodas:
      // seguir em frente, sem correcao PD.
      pwmEsquerdo = Config::VELOCIDADE_BASE;
      pwmDireito = Config::VELOCIDADE_BASE;

      Motores::aplicarComandos(pwmEsquerdo, pwmDireito);

    } else {
      // Se a faixa larga persistir, parar por seguranca.
      Motores::frear();
    }

  } else if (leitura.linhaDetectada) {

    // Linha normal: usar o controlador PD.
    cruzamentoAtivo = false;
    linhaAnteriorValida = true;
    recuperandoLinha = false;

    // Memorizar apenas deslocamentos significativos
    if (leitura.erro <= -ERRO_LATERAL_MINIMO) {
        ultimoLadoLinha = -1;
        instanteUltimoLado = millis();
    }
    else if (leitura.erro >= ERRO_LATERAL_MINIMO) {
        ultimoLadoLinha = +1;
        instanteUltimoLado = millis();
    }

    pd = ControlePD::calcular(leitura.erro);

    Motores::calcularComandos(
      pd.correcao,
      pwmEsquerdo,
      pwmDireito
    );

    Motores::aplicarComandos(pwmEsquerdo, pwmDireito);

  } else {

    // Nenhum sensor encontrou a linha.
    cruzamentoAtivo = false;
    linhaAnteriorValida = false;

    ControlePD::reiniciar();

    if (!recuperandoLinha) {
        recuperandoLinha = true;
        inicioRecuperacao = millis();
    }

    bool ladoRecente = ultimoLadoLinha != 0 && millis() - instanteUltimoLado <= MEMORIA_LADO_MS;

    bool dentroDoTempo = millis() - inicioRecuperacao < TEMPO_RECUPERACAO_MS;

    if (ladoRecente && dentroDoTempo) {

        if (ultimoLadoLinha == -1) {
            // Linha estava à esquerda:
            // esquerda para trás, direita para frente.
            pwmEsquerdo = -PWM_RECUPERACAO;
            pwmDireito = PWM_RECUPERACAO;
        } else {
            // Linha estava à direita:
            // esquerda para frente, direita para trás.
            pwmEsquerdo = PWM_RECUPERACAO;
            pwmDireito = -PWM_RECUPERACAO;
        }

        Motores::aplicarComandos(
            pwmEsquerdo,
            pwmDireito
        );

    } else {
        // Sem direção confiável ou tempo esgotado.
        Motores::frear();
    }

  }


  static unsigned long ultimoDebug = 0;
  if (millis() - ultimoDebug >= Config::INTERVALO_DEBUG_MS) {
    ultimoDebug = millis();
    for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
      Serial.print(F("S")); Serial.print(i + 1);
      Serial.print(F(":")); Serial.print(leitura.preto[i]); Serial.print(' ');
    }
    Serial.print(F(" | pos=")); Serial.print(leitura.posicao);
    Serial.print(F(" erro=")); Serial.print(leitura.erro);
    Serial.print(F(" | P=")); Serial.print(pd.proporcional, 1);
    Serial.print(F(" D=")); Serial.print(pd.derivativo, 1);
    Serial.print(F(" dE/dt=")); Serial.print(pd.derivadaErro, 1);
    Serial.print(F(" corr=")); Serial.print(pd.correcao, 1);
    Serial.print(F(" | PWM E=")); Serial.print(pwmEsquerdo);
    Serial.print(F(" D=")); Serial.print(pwmDireito);
    if (!leitura.linhaDetectada) Serial.print(F(" | SEM LINHA"));
    if (leitura.faixaLarga) Serial.print(F(" | FAIXA LARGA"));
    if (Motores::movimentacaoLiberada()) {
      Serial.println(F(" | MOTORES HABILITADOS"));
    } else {
      Serial.println(F(" | MOTORES BLOQUEADOS"));
    }
  }
}
