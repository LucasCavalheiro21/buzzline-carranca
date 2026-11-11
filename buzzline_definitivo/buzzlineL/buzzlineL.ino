// Luiz Gonzigzaga
// ESP32 Dev Module
// placa: ESP32 Dev Module

// led piscando = calibrando
// led apagado = pronto, aperte o botao
// led aceso = falha na calibracao: apertar o reset

#include "mapeamento.h"

void setup(){

  iniciarPinos();
  controlarMotores(0, 0);
  if (!calibrarSensores()){
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
  Serial.println("Botao pressionado");

  if (roboLigado){
    lerSensores();
    controlePDI();
  } else {
    controlarMotores(0, 0);
  }
}