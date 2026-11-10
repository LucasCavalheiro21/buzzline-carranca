// Luiz Gonzigzaga
// placa: ESP32 Dev Module

// led piscando = calibrando (passar os dois sensores na linha e no chao)
// led apagado = pronto, aperte o botao
// led aceso = andando
// led aceso logo apos calibrar = falha, apertar o reset
// robo parou e led apagou = perdeu a linha ou saiu da pista, recolocar e apertar o botao

#include "mapeamento.h"

void setup(){

  iniciarPinos();
  controlarMotores(0, 0);

  if (!calibrarSensores()){

    digitalWrite(PINO_LED, HIGH);

    while (true){

      delay(100);

    }
  }

}

void loop(){

  verificarBotao();

  if (roboLigado){

    seguirLinha();

  } else {

    controlarMotores(0, 0);

  }
}
