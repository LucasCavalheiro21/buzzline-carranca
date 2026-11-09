// placa: ESP32 Dev Module

// led apagado = parado, aperte o botao
// led aceso = andando

#include "mapeamento.h"

void setup(){

  iniciarPinos();
  controlarMotores(0, 0);

}

void loop(){

  verificarBotao();
  lerSensores();

  if (roboLigado){

    seguirLinha();

  } else {

    controlarMotores(0, 0);

  }
}
