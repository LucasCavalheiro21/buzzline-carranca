// placa: ESP32 Dev Module

/* código simplificado (sem PID)

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

*/

// Luiz Gonzigzaga

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
    controlePD();

  } else {

    controlarMotores(0, 0);

  }
}
