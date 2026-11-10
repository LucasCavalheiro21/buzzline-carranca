#pragma once
#include <Arduino.h>
#include "Configuracoes.h"

namespace Sensores {
  struct Leituras {
    uint16_t bruto[Config::NUM_SENSORES];
    uint16_t preto[Config::NUM_SENSORES]; // 0 branco ... 1000 preto
    uint16_t posicao;                  // 0 (S1) ... 6000 (S7)
    int16_t erro;                      // posicao - 3000
    bool linhaDetectada;
    bool faixaLarga;
  };

  void iniciar();
  void lerBrutos(uint16_t valores[Config::NUM_SENSORES]);
  bool calibrar(const uint16_t branco[Config::NUM_SENSORES],
               const uint16_t preto[Config::NUM_SENSORES]);
  Leituras ler();
}
