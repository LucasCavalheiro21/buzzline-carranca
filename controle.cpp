#include "mapeamento.h"

// variaveis

int leituraEsq = 0; // 0 a 100 | 100 = vendo a linha | 0 = vendo o chao
int minEsq = 4095; // menor valor lido na calibracao
int maxEsq = 0; // maior valor lido na calibracao

int leituraDir = 0;
int minDir = 4095;
int maxDir = 0;

bool roboLigado = false;

RockyMotors motores; // A = esquerdo | B = direito

int erroAnterior = 0; // erro do ultimo calculo (usado no KD)
unsigned long ultimoControle = 0; // quando foi o ultimo calculo

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

// calibracao: guarda o menor e o maior valor que cada sensor leu

bool calibrarSensores(){

  unsigned long inicio = millis();

  while (millis() - inicio < TEMPO_CALIBRACAO){

    int esq = analogRead(PINO_SENSOR_ESQ);
    int dir = analogRead(PINO_SENSOR_DIR);

    minEsq = min(minEsq, esq);
    maxEsq = max(maxEsq, esq);
    minDir = min(minDir, dir);
    maxDir = max(maxDir, dir);

    digitalWrite(PINO_LED, (millis() / 250) % 2); // pisca o led

    delay(2);

  }

  digitalWrite(PINO_LED, LOW);

  // so vale se os dois sensores viram o preto e o branco

  return (maxEsq - minEsq >= CONTRASTE_MINIMO) && (maxDir - minDir >= CONTRASTE_MINIMO);

}

// sensores

// converte a leitura para 0 a 100 (100 = linha | 0 = chao)

int normalizar(int bruto, int minimo, int maximo){

  int valor = constrain(map(bruto, minimo, maximo, 0, 100), 0, 100);

  // deixa 100 = preto

  if (!PRETO_E_VALOR_ALTO){
    valor = 100 - valor;
  }

  // deixa 100 = linha

  if (LINHA_BRANCA){
    valor = 100 - valor;
  }

  return valor;

}

void lerSensores(){

  leituraEsq = normalizar(analogRead(PINO_SENSOR_ESQ), minEsq, maxEsq);
  leituraDir = normalizar(analogRead(PINO_SENSOR_DIR), minDir, maxDir);

}

// motores

void controlarMotores(int velEsq, int velDir){

  // converte de -255/255 para -100/100 (escala da biblioteca)

  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);

}

// controle PD

void controlePD(){

  // so calcula a cada INTERVALO_CONTROLE ms (senao o KD nao funciona)

  if (millis() - ultimoControle < INTERVALO_CONTROLE){
    return;
  }

  ultimoControle = millis();

  // positivo = linha a esquerda | negativo = linha a direita | 0 = centralizado

  int erro = leituraEsq - leituraDir;

  // P corrige o erro de agora | D freia se o erro estiver mudando rapido

  int correcao = KP * erro + KD * (erro - erroAnterior);
  erroAnterior = erro;

  // uma roda desacelera e a outra acelera para virar

  int velEsq = constrain(VELOCIDADE_BASE - correcao, VELOCIDADE_MINIMA, 255);
  int velDir = constrain(VELOCIDADE_BASE + correcao, VELOCIDADE_MINIMA, 255);

  controlarMotores(velEsq, velDir);

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
      erroAnterior = 0; // esquece o erro antigo

    }
  }

  ultimaLeituraBotao = leitura;

}