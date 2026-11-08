#include "mapeamento.h"

/* código simplificado (sem PID)

// variaveis

int leituraEsq = 0; // 0 a 4095 | preto = baixo | branco = alto
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

  pinMode(PINO_BOTAO, INPUT); // solto = HIGH | apertado = LOW
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);

}

// sensores

void lerSensores(){

  leituraEsq = analogRead(PINO_SENSOR_ESQ);
  leituraDir = analogRead(PINO_SENSOR_DIR);

}

// motores

void controlarMotores(int velEsq, int velDir){

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);

}

// seguir a linha: reto ou curva com forca fixa

void seguirLinha(){

  // positivo = linha a esquerda | negativo = linha a direita | perto de 0 = centralizado

  int erro = leituraEsq - leituraDir;

  // linha preta le menos que o chao: inverte o sinal

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

  // continua virando enquanto ve a linha e por mais TEMPO_MEMORIA ms

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

*/

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

  pinMode(PINO_BOTAO, INPUT); // solto = HIGH | apertado = LOW
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);

}

// calibrar
// calibracao: guarda o menor e o maior valor que cada sensor leu

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

  // agora 100 = preto | 0 = branco
  // deixa 100 = preto

  if (!PRETO_E_VALOR_ALTO){
    valor = 100 - valor;
  }

  // agora 100 = linha | 0 = chao
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

  // velocidade positiva = frente | negativa = re | 0 = parado (-255 a 255)
  // a biblioteca da rocky usa -100 a 100
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

  // erro > 0 = sensor ESQUERDO ve a linha -> linha a esquerda -> vira para a esquerda
  // erro < 0 = sensor DIREITO ve a linha  -> linha a direita  -> vira para a direita
  // erro = 0 = os dois veem o chao -> centralizado -> segue reto
  // positivo = linha a esquerda | negativo = linha a direita | 0 = centralizado

  int erro = leituraEsq - leituraDir;

  // P corrige o erro de agora | D freia se o erro estiver mudando rapido

  int correcao = KP * erro + KD * (erro - erroAnterior);
  erroAnterior = erro;

  // limita entre VELOCIDADE_MINIMA e 255
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
