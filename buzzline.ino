// ESP32 Dev Module

#include "mapeamento.h"

void setup(){

  iniciarPinos();
  controlarMotores(0, 0);

  // durante a calibracao, passar os dois sensores sobre a linha e chao

  if (!calibrarSensores()){

    // led aceso fixo = FALHA | apertar o reset da placa para calibrar de novo

    digitalWrite(PINO_LED, HIGH);

    while (true){
      
      delay(100);

    }
  }

  // led apagado = calibrou certo, aperte o botao para ligar o robo

}

void loop(){

  verificarBotao();
  lerSensores();

  if (roboLigado){

    controlePD();

  } else {

    controlarMotores(0, 0); // desligado = parado (tambem para na hora quando aperta o botao)

  }
}