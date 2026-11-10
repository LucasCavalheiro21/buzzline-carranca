#pragma once
#include <Arduino.h>
namespace Motores {
  void iniciar();
  void parar(); // LOW/LOW, parada livre
  void frear(); // HIGH/HIGH, se movimentacao estiver liberada
  bool movimentacaoLiberada();
  void calcularComandos(float correcao, int &pwmEsquerdo, int &pwmDireito);
  bool aplicarComandos(int pwmEsquerdo, int pwmDireito);
}
