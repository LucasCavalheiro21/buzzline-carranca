#include "mapeamento.h"

// variaveis

int minEsq = 4095, maxEsq = 0; // calibracao do sensor esquerdo
int minDir = 4095, maxDir = 0; // calibracao do sensor direito
int leituraEsq = 0; // valor normalizado (0 a 100)
int leituraDir = 0; // 100 = em cima da LINHA, 0 = em cima do CHAO
bool roboLigado = false; // true = andando, false = parado

RockyMotors motores; // A = esquerdo | B = direito

// memoria do controle

int erroAnterior = 0; // erro do calculo anterior 
unsigned long ultimoControle = 0; // ultima vez que o controle foi calculado

// memoria do botao

bool ultimaLeituraBotao = HIGH;
bool estadoBotao = HIGH;
unsigned long instanteMudancaBotao = 0;
unsigned long ultimoPrintSensores = 0; // ultima vez que mostrou os sensores no serial
unsigned long ultimoPrintMotores = 0; // ultima vez que mostrou os motores no serial

// pinos

void iniciarPinos(){

  pinMode(PINO_BOTAO, INPUT_PULLUP); // botao BOOT: LOW quando apertado
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW); // comeca apagado

  ultimaLeituraBotao = digitalRead(PINO_BOTAO);
  estadoBotao = ultimaLeituraBotao;

  Serial.println("pinos configurados");

}

// calibrar

bool calibrarSensores(){

  // led pisca enqto calibra

  Serial.println(LINHA_BRANCA ? "arena: linha BRANCA em chao PRETO" : "arena: linha PRETA em chao BRANCO");
  Serial.println("calibracao iniciada: passe os sensores sobre a linha e chao");

  unsigned long inicio = millis(); 

  while (millis() - inicio < TEMPO_CALIBRACAO){

    int esq = analogRead(PINO_SENSOR_ESQ);
    int dir = analogRead(PINO_SENSOR_DIR);

    if (esq < minEsq) minEsq = esq; 
    if (esq > maxEsq) maxEsq = esq; 
    if (dir < minDir) minDir = dir;
    if (dir > maxDir) maxDir = dir;

    digitalWrite(PINO_LED, (millis() / 250) % 2); // pisca o led

    delay(2);

  }

  digitalWrite(PINO_LED, LOW);

  // mostra o resultado da calibracao

  Serial.println("calibracao concluida");
  Serial.print("sensor esquerdo -> min: ");
  Serial.print(minEsq);
  Serial.print(" | max: ");
  Serial.println(maxEsq);
  Serial.print("sensor direito  -> min: ");
  Serial.print(minDir);
  Serial.print(" | max: ");
  Serial.println(maxDir);

  // confere se cada sensor viu o preto e branco

  bool ok = true;

  if (maxEsq - minEsq < CONTRASTE_MINIMO){

    Serial.println("[FALHA] sensor ESQUERDO nao viu diferenca entre linha e chao");
    ok = false;

  }

  if (maxDir - minDir < CONTRASTE_MINIMO){

    Serial.println("[FALHA] sensor DIREITO nao viu diferenca entre linha e chao");
    ok = false;

  }

  return ok;

}

// sensores

void lerSensores() 

  int brutoEsq = analogRead(PINO_SENSOR_ESQ);
  int brutoDir = analogRead(PINO_SENSOR_DIR);

  // converte para 0 a 100 

  leituraEsq = constrain(map(brutoEsq, minEsq, maxEsq, 0, 100), 0, 100);
  leituraDir = constrain(map(brutoDir, minDir, maxDir, 0, 100), 0, 100);

  // 100 = preto | 0 = branco

  if (!PRETO_E_VALOR_ALTO){

    leituraEsq = 100 - leituraEsq;
    leituraDir = 100 - leituraDir;

  }

  // 100 = LINHA | 0 = CHAO

  if (LINHA_BRANCA){

    leituraEsq = 100 - leituraEsq;
    leituraDir = 100 - leituraDir;

  }

  if (!roboLigado && millis() - ultimoPrintSensores >= TEMPO_SERIAL){

    ultimoPrintSensores = millis();

    Serial.print("bruto esq: ");
    Serial.print(brutoEsq);
    Serial.print(" | bruto dir: ");
    Serial.print(brutoDir);
    Serial.print(" | linha esq: ");
    Serial.print(leituraEsq);
    Serial.print(" | linha dir: ");
    Serial.println(leituraDir);

  }
}

// motores

void controlarMotores(int velEsq, int velDir){

  // velocidade positiva = frente | negativa = re | 0 = parado (-255 a 255)

  velEsq = constrain(velEsq, -255, 255);
  velDir = constrain(velDir, -255, 255);

  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ); // motor esquerdo (entrada A)
  motores.setSpeedRight(velDir * SENTIDO_DIR); // motor direito (entrada B)

}

// controles

void reiniciarControle(){
  erroAnterior = 0;
  ultimoControle = 0;

}

void controlePD(){

  if (millis() - ultimoControle < (unsigned long)INTERVALO_CONTROLE) return;
  ultimoControle = millis();

  // erro > 0 = sensor ESQUERDO ve a linha -> linha a esquerda -> vira para a esquerda
  // erro < 0 = sensor DIREITO ve a linha  -> linha a direita  -> vira para a direita
  // erro = 0 = os dois veem o chao -> centralizado -> segue reto

  int erro = leituraEsq - leituraDir;

  // P = reage ao erro de agora | D = reage a VELOCIDADE com que o erro muda

  float correcao = KP * erro + KD * (erro - erroAnterior);
  erroAnterior = erro;

  int velEsq = VELOCIDADE_BASE - (int)correcao;
  int velDir = VELOCIDADE_BASE + (int)correcao;

  velEsq = constrain(velEsq, VELOCIDADE_MINIMA, 255);
  velDir = constrain(velDir, VELOCIDADE_MINIMA, 255);

  controlarMotores(velEsq, velDir);

  if (millis() - ultimoPrintMotores >= TEMPO_SERIAL){

    ultimoPrintMotores = millis();

    Serial.print("esq: ");
    Serial.print(leituraEsq);
    Serial.print(" | dir: ");
    Serial.print(leituraDir);
    Serial.print(" | erro: ");
    Serial.print(erro);
    Serial.print(" | vel esq: ");
    Serial.print(velEsq);
    Serial.print(" | vel dir: ");
    Serial.println(velDir);

  }
}

// botao

void verificarBotao(){

  bool leitura = digitalRead(PINO_BOTAO);

  if (leitura != ultimaLeituraBotao) instanteMudancaBotao = millis();

  if (millis() - instanteMudancaBotao >= (unsigned long)TEMPO_DEBOUNCE && leitura != estadoBotao){

    estadoBotao = leitura;

    if (estadoBotao == LOW) {

      roboLigado = !roboLigado;
      digitalWrite(PINO_LED, roboLigado); // led aceso = ligado

      if (roboLigado){

        reiniciarControle();
        Serial.println("botao apertado: robo LIGADO");

      } else {

        controlarMotores(0, 0);
        Serial.println("botao apertado: robo DESLIGADO");

      }
    }
  }

  ultimaLeituraBotao = leitura;
  
}
