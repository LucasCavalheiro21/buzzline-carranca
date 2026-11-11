#include "mapeamento.h"

// variaveis

int leituraEsq = 0; int leituraDir = 0; 
int minEsq = 4095; int maxEsq = 0; 
int minDir = 4095; int maxDir = 0; 

float P;
float D;
float I;

bool roboLigado = false;

RockyMotors motores; 

int erroAnterior = 0; // erro do ultimo calculo (usado no KD)
int erroAcumulado = 0; // acumulador da correcao integral
unsigned long ultimoControle = 0; // quando foi o ultimo calculo
float integralErro = 0; // soma dos erros (usado no KI)

// memoria do botao
bool ultimaLeituraBotao = HIGH;
bool estadoBotao = HIGH;
unsigned long instanteMudancaBotao = 0;

// pinos
void iniciarPinos(){
  pinMode(PINO_BOTAO, INPUT); 
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);
}

// calibracao
bool calibrarSensores(){
  unsigned long inicio = millis();
  bool sinalDisparado = false;

  while (millis() - inicio < TEMPO_CALIBRACAO){

    unsigned long tempoDecorrido = millis() - inicio;

    if (tempoDecorrido >= 5000 && !sinalDisparado) {

      digitalWrite(PINO_LED, HIGH);
      delay(1000); // Mantém o LED aceso fixo por 1 segundo para você mover o robô
  
      inicio += 1000; 
      sinalDisparado = true;
      continue; // Vai para a próxima iteração do while

    }

    // LEITURA DOS SENSORES
    int esq = analogRead(PINO_SENSOR_ESQ);
    int dir = analogRead(PINO_SENSOR_DIR);
    // Guarda o menor e o maior valor lido de cada sensor
    minEsq = min(minEsq, esq);
    maxEsq = max(maxEsq, esq);
    minDir = min(minDir, dir);
    maxDir = max(maxDir, dir);

    if (!sinalDisparado) {
      digitalWrite(PINO_LED, (millis() / 250) % 2); // Pisca padrão (250ms)
    } else {
      digitalWrite(PINO_LED, (millis() / 125) % 2); // Pisca mais rápido (125ms) para avisar que é a reta final
    }
    delay(2);
  }

  digitalWrite(PINO_LED, LOW); // Desliga o LED ao finalizar

  // Confere se os dois sensores viram contraste suficiente entre o preto e o branco
  return (maxEsq - minEsq >= CONTRASTE_MINIMO) && (maxDir - minDir >= CONTRASTE_MINIMO);
}

// sensores
// transforma o valor bruto em 0 a 100 (100 = LINHA | 0 = CHAO)
// serve para os dois sensores, so muda o min e max
// converte a leitura para 0 a 100 (100 = linha | 0 = chao)
int normalizar(int bruto, int minimo, int maximo){
  int valor = constrain(map(bruto, minimo, maximo, 0, 100), 0, 100);

  if (!PRETO_E_VALOR_ALTO){
    valor = 100 - valor;
  }

  if (LINHA_BRANCA){
    valor = 100 - valor;
  }

  return valor;
}

void lerSensores(){
  leituraEsq = normalizar(analogRead(PINO_SENSOR_ESQ), minEsq, maxEsq);
  leituraDir = normalizar(analogRead(PINO_SENSOR_DIR), minDir, maxDir);
}

// botao
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
      integralErro = 0;
    }
  }
  ultimaLeituraBotao = leitura;
}

// motores
void controlarMotores(int velEsq, int velDir){
  // converte de -255/255 para -100/100 (escala da biblioteca)
  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);
}

// controle PDI
void controlePDI(){
  // so calcula a cada INTERVALO_CONTROLE ms 
  if (millis() - ultimoControle < INTERVALO_CONTROLE){
    return;
  }

  ultimoControle = millis();

  int erro = leituraEsq - leituraDir;
  integralErro = constrain(integralErro + erro, -LIMITE_INTEGRAL, LIMITE_INTEGRAL);

  P = KP * erro;
  D = KD * (erro - erroAnterior);
  I = KI * integralErro;
  int correcao = P + I + D;

  erroAnterior = erro;

  // limita entre VELOCIDADE_MINIMA e 255
  // uma roda desacelera e a outra acelera para virar
  int velEsq = constrain(VELOCIDADE_BASE - correcao, -VELOCIDADE_BASE, VELOCIDADE_BASE);
  int velDir = constrain(VELOCIDADE_BASE + correcao, -VELOCIDADE_BASE, VELOCIDADE_BASE);

  controlarMotores(velEsq, velDir);
}