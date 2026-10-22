#include "buzzline.h"

void setup() {

  Serial.begin(115200);
  iniciarPinos();
  calibrarSensores();

}

void loop() {
  verificarBotao();     // liga/desliga

  if (roboLigado) {

    lerSensores();
    controleProporcional();

  } else {

    controlarMotores(0, 0);   // parado
    
  }
}