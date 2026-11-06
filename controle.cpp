#include "mapeamento.h"

// variaveis

int leituraEsq = 0; // 100 = em cima da LINHA, 0 = em cima do CHAO
int minEsq = 4095;
int maxEsq = 0; // calibracao do sensor esquerdo

int leituraDir = 0;
int minDir = 4095;
int maxDir = 0; // calibracao do sensor direito

bool roboLigado = false; // true = andando, false = parado

RockyMotors motores; // A = esquerdo | B = direito

int erroAnterior = 0; // erro do calculo anterior
unsigned long ultimoControle = 0; // ultima vez que o controle foi calculado

// memoria do botao

bool ultimaLeituraBotao = HIGH;
bool estadoBotao = HIGH;
unsigned long instanteMudancaBotao = 0;

// pinos

void iniciarPinos(){

  pinMode(PINO_BOTAO, INPUT_PULLUP); // botao BOOT: LOW quando apertado
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW); // comeca apagado

}

// calibrar

bool calibrarSensores(){

  // led pisca enqto calibra

  unsigned long inicio = millis();

  while (millis() - inicio < TEMPO_CALIBRACAO){

    int esq = analogRead(PINO_SENSOR_ESQ);
    int dir = analogRead(PINO_SENSOR_DIR);

    // guarda o menor e o maior valor lido de cada sensor

    minEsq = min(minEsq, esq);
    maxEsq = max(maxEsq, esq);
    minDir = min(minDir, dir);
    maxDir = max(maxDir, dir);

    digitalWrite(PINO_LED, (millis() / 250) % 2); // pisca o led

    delay(2);

  }

  digitalWrite(PINO_LED, LOW);

  // confere se os dois sensores viram o preto e o branco
  // true = calibrou certo | false = falhou

  return (maxEsq - minEsq >= CONTRASTE_MINIMO) && (maxDir - minDir >= CONTRASTE_MINIMO);

}

// sensores

// transforma o valor bruto em 0 a 100 (100 = LINHA | 0 = CHAO)
// serve para os dois sensores, so muda o min e max

int normalizar(int bruto, int minimo, int maximo){

  int valor = constrain(map(bruto, minimo, maximo, 0, 100), 0, 100);

  // agora 100 = preto | 0 = branco

  if (!PRETO_E_VALOR_ALTO){
    valor = 100 - valor;
  }

  // agora 100 = linha | 0 = chao

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

  // velocidade positiva = frente | negativa = re | 0 = parado (-255 a 255)
  // a biblioteca da rocky usa -100 a 100

  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ); // motor esquerdo (entrada A)
  motores.setSpeedRight(velDir * SENTIDO_DIR); // motor direito (entrada B)

}

// controle

void controlePD(){

  // so calcula a cada INTERVALO_CONTROLE ms

  if (millis() - ultimoControle < INTERVALO_CONTROLE){
    return;
  }

  ultimoControle = millis();

  // erro > 0 = sensor ESQUERDO ve a linha -> linha a esquerda -> vira para a esquerda
  // erro < 0 = sensor DIREITO ve a linha  -> linha a direita  -> vira para a direita
  // erro = 0 = os dois veem o chao -> centralizado -> segue reto

  int erro = leituraEsq - leituraDir;

  // P = reage ao erro de agora | D = reage a VELOCIDADE com que o erro muda

  int correcao = KP * erro + KD * (erro - erroAnterior);
  erroAnterior = erro;

  // limita entre VELOCIDADE_MINIMA e 255

  int velEsq = constrain(VELOCIDADE_BASE - correcao, VELOCIDADE_MINIMA, 255);
  int velDir = constrain(VELOCIDADE_BASE + correcao, VELOCIDADE_MINIMA, 255);

  controlarMotores(velEsq, velDir);

}

// botao

void verificarBotao(){

  bool leitura = digitalRead(PINO_BOTAO);

  // o sinal mudou: comeca a contar o tempo

  if (leitura != ultimaLeituraBotao){

    instanteMudancaBotao = millis();
    
  }

  // so aceita a mudanca se o sinal ficou parado por TEMPO_DEBOUNCE ms

  if (millis() - instanteMudancaBotao >= TEMPO_DEBOUNCE && leitura != estadoBotao){

    estadoBotao = leitura;

    if (estadoBotao == LOW) {

      roboLigado = !roboLigado;
      digitalWrite(PINO_LED, roboLigado); // led aceso = ligado
      erroAnterior = 0; // comeca o controle do zero

    }
  }

  ultimaLeituraBotao = leitura;

}