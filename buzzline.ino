// programa principal do robo seguidor de linha

// selecionar "ESP32 Dev Module" na arduino ide



#include "mapeamento.h" // inclui as funcoes de controle do robo



void setup() {



  Serial.begin(115200); // inicia o monitor serial (usar 115200 no monitor da ide)

  Serial.println("robo iniciando...");



  iniciarPinos(); // configura entradas e saidas

  calibrarSensores(); // para calibrar, deve passar os sensores sobre a linha e o chao branco



  // apos calibrar, o robo espera o botao para comecar



  Serial.println("pronto! aperte o botao para ligar o robo");



}



void loop() {



  verificarBotao(); // ve o estado do botao



  if (roboLigado) {



    // se o robo estiver ligado, ele le os sensores e ajusta os motores



    lerSensores(); // atualiza leituraEsq e leituraDir

    controleProporcional(); // calcula o erro e ajusta motores



  } else {



    // se o robo estiver desligado, ele para os motores



    controlarMotores(0, 0);



  }

}
