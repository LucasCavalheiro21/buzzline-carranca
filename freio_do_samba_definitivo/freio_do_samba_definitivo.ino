// Freio do Samba
// placa: ESP32 Dev Module

// LED:
// piscando = calibrando (passar os dois sensores na linha e no chao)
// apagado = pronto, aperte o botao
// aceso = andando
// aceso logo apos calibrar = calibracao falhou, apertar o reset

#include "mapeamento.h" // pinos, ajustes e funcoes

// roda uma vez, quando liga a placa

void setup(){

  iniciarPinos(); // configura botao e LED
  controlarMotores(0, 0); // garante os motores parados

  if (!calibrarSensores()){ // calibra os sensores; se falhar, trava aqui

    digitalWrite(PINO_LED, HIGH); // LED aceso = falha
    while (true){ // fica parado ate apertar o reset

      delay(100);

    }
  }
}

// roda sem parar

void loop(){

  verificarBotao(); // liga ou desliga o robo

  if (roboLigado){

    lerSensores(); // le os dois sensores
    controlePDI(); // calcula e manda a velocidade das rodas

  } else {

    controlarMotores(0, 0); // desligado: motores parados
    
  }
}