#include "Sensores.h"
#include <QTRSensors.h>

namespace {
  QTRSensors qtr;
  uint16_t referenciaBranco[Config::NUM_SENSORES] = {};
  uint16_t referenciaPreto[Config::NUM_SENSORES] = {};
  bool calibrado = false;

  uint16_t normalizar(uint16_t valor, uint16_t branco, uint16_t preto) {
    // calibrar() ja assegura preto > branco, com contraste suficiente.
    int32_t x = (int32_t(valor) - int32_t(branco)) * 1000L /
                (int32_t(preto) - int32_t(branco));
    if (x < 0) return 0;
    if (x > 1000) return 1000;
    return static_cast<uint16_t>(x);
  }
}

void Sensores::iniciar() {
  qtr.setTypeRC();
  qtr.setSensorPins(Config::PINOS_SENSORES, Config::NUM_SENSORES);
  qtr.setTimeout(Config::TEMPO_LIMITE_RC_US);
}

void Sensores::lerBrutos(uint16_t valores[Config::NUM_SENSORES]) {
  qtr.read(valores); // Tempo de descarga dos capacitores, em microssegundos.
}

bool Sensores::calibrar(const uint16_t branco[Config::NUM_SENSORES],
                       const uint16_t preto[Config::NUM_SENSORES]) {
  for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
    // QTR-8RC: normalmente preto demora MAIS para descarregar.
    if (preto[i] < Config::PRETO_MINIMO_US ||
        int32_t(preto[i]) - int32_t(branco[i]) < Config::CONTRASTE_MINIMO_US) {
      calibrado = false;
      return false;
    }
  }
  for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
    referenciaBranco[i] = branco[i];
    referenciaPreto[i] = preto[i];
  }
  calibrado = true;
  return true;
}

Sensores::Leituras Sensores::ler() {
  Leituras r = {};
  lerBrutos(r.bruto);
  if (!calibrado) return r;

  uint32_t somaPesos = 0;
  uint32_t somaIntensidades = 0;
  uint16_t maisPreto = 0;
  uint8_t canaisEscuros = 0;

  for (uint8_t i = 0; i < Config::NUM_SENSORES; ++i) {
    r.preto[i] = 1000 - normalizar(r.bruto[i], referenciaBranco[i], referenciaPreto[i]);
    somaIntensidades += r.preto[i];
    somaPesos += uint32_t(i) * 1000UL * r.preto[i];
    if (r.preto[i] > maisPreto) maisPreto = r.preto[i];
    if (r.preto[i] >= Config::PRETO_FAIXA_LARGA) canaisEscuros++;
  }

  r.linhaDetectada = maisPreto >= Config::MINIMO_PRETO_PARA_LINHA;
  r.faixaLarga = canaisEscuros >= Config::MIN_SENSORES_FAIXA_LARGA;

  if (r.linhaDetectada && somaIntensidades > 0) {
    r.posicao = static_cast<uint16_t>(somaPesos / somaIntensidades);
    if (Config::INVERTER_POSICAO) r.posicao = 6000 - r.posicao;
    r.erro = static_cast<int16_t>(r.posicao) - 3000;
  } else {
    r.posicao = 3000;
    r.erro = 0;
  }
  return r;
}
