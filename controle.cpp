#include "mapeamento.h"

// variaveis

int leituraEsq = 0; // 0 a 4095 | preto = valor baixo | branco = valor alto
int leituraDir = 0;

bool roboLigado = false;

RockyMotors motores; // A = esquerdo | B = direito

int ultimoLado = 0; // 1 = linha estava a esquerda | -1 = a direita
unsigned long instanteViuLinha = 0; // ultima vez que um sensor viu a linha

// memoria do botao

bool ultimaLeituraBotao = HIGH;
bool estadoBotao = HIGH;
unsigned long instanteMudancaBotao = 0;

// pinos

void iniciarPinos(){

  pinMode(PINO_BOTAO, INPUT_PULLUP); // solto = HIGH | apertado = LOW
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);

}

// sensores

void lerSensores(){

  leituraEsq = analogRead(PINO_SENSOR_ESQ);
  leituraDir = analogRead(PINO_SENSOR_DIR);

}

// motores (-100 a 100, mesma escala da biblioteca)

void controlarMotores(int velEsq, int velDir){

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);

}

// seguir a linha: reto ou curva com forca fixa

void seguirLinha(){

  // positivo = linha a esquerda | negativo = linha a direita | perto de 0 = centralizado

  int erro = leituraEsq - leituraDir;

  // linha preta: o sensor na linha le MENOS, entao inverte o sinal

  if (!LINHA_BRANCA){
    erro = -erro;
  }

  // um sensor ve a linha: guarda o lado e a hora

  if (erro > ZONA_MORTA){

    ultimoLado = 1;
    instanteViuLinha = millis();

  } else if (erro < -ZONA_MORTA){

    ultimoLado = -1;
    instanteViuLinha = millis();

  }

  // vendo a linha agora ou ha menos de TEMPO_MEMORIA ms: continua virando

  bool virando = millis() - instanteViuLinha < TEMPO_MEMORIA;

  if (virando && ultimoLado == 1){

    controlarMotores(VEL_CURVA_DENTRO, VEL_CURVA_FORA); // vira para a esquerda

  } else if (virando && ultimoLado == -1){

    controlarMotores(VEL_CURVA_FORA, VEL_CURVA_DENTRO); // vira para a direita

  } else {

    controlarMotores(VEL_RETA_ESQ, VEL_RETA_DIR); // segue reto

  }
}

// botao: cada aperto liga ou desliga o robo

void verificarBotao(){

  bool leitura = digitalRead(PINO_BOTAO);

  // o sinal mudou: comeca a contar o tempo

  if (leitura != ultimaLeituraBotao){

    instanteMudancaBotao = millis();

  }

  // so aceita se o sinal ficou estavel por TEMPO_DEBOUNCE (ignora o tremor do botao)

  if (millis() - instanteMudancaBotao >= TEMPO_DEBOUNCE && leitura != estadoBotao){

    estadoBotao = leitura;

    if (estadoBotao == LOW) {

      roboLigado = !roboLigado;
      digitalWrite(PINO_LED, roboLigado); // led aceso = ligado
      ultimoLado = 0; // esquece a ultima curva

    }
  }

  ultimaLeituraBotao = leitura;

}
