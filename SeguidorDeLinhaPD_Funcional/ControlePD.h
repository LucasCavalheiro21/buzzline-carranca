#pragma once
#include <Arduino.h>

namespace ControlePD {
  struct Resultado {
    float proporcional;
    float derivativo;
    float derivadaErro; // unidades de erro por segundo, depois do filtro
    float correcao;     // parcela P + D (limitada)
  };

  // Zerar memoria do D ao desligar, perder a linha ou detectar faixa larga.
  void reiniciar();
  Resultado calcular(int16_t erro);
}
