// ESP32 Dev Module

#include "mapeamento.h"

void setup(){

  Serial.begin(115200);
  Serial.println("robo iniciando...");

  iniciarPinos();
  controlarMotores(0, 0);

  // durante a calibracao, passar os dois sensores sobre a linha e chao

  if (!calibrarSensores()){

    // led aceso = FALHA

    Serial.println("calibrar de novo");
    digitalWrite(PINO_LED, HIGH);

    while (true){
      
      delay(100);

    }
  }

  Serial.println("calibracao concluida, aperte o botao para ligar o robo");

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