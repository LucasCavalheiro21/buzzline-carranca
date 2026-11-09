// placa: ESP32 Dev Module

// led piscando = calibrando
// led apagado = pronto, aperte o botao
// led aceso = andando (ou, logo apos calibrar, FALHA: apertar o reset)

#include "mapeamento.h"

void setup(){

  iniciarPinos();
  controlarMotores(0, 0);

  // durante a calibracao, passar os dois sensores na linha e no chao

  if (!calibrarSensores()){

    // led aceso fixo = FALHA | apertar o reset da placa para calibrar de novo
    // falhou: led aceso e o robo fica travado aqui

    digitalWrite(PINO_LED, HIGH);

    while (true){
      

      delay(100);

    }
  }

  // led apagado = calibrou certo, aperte o botao para ligar o robo

}

void loop(){

  verificarBotao();

  if (roboLigado){
    
    lerSensores();
    controlePID();

  } else {

    controlarMotores(0, 0);

  }
}
