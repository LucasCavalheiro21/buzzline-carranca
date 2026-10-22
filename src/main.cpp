// Buzzline - robo seguidor de linha com a RoboCore BlackBoard Rocky
// (no PlatformIO o antigo buzzline.ino vira este main.cpp)

#include "buzzline.h"

void setup() {
  Serial.begin(115200);
  iniciarPinos();
  calibrarSensores();   // passe os sensores sobre a linha e o chao branco
  // Apos calibrar, o robo espera o botao BOOT para comecar
}

void loop() {
  verificarBotao();     // liga/desliga

  if (robotLigado) {
    lerSensores();
    controleProporcional();
  } else {
    controlarMotores(0, 0);   // parado
  }
}