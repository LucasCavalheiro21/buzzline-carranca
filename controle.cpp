#include "mapeamento.h" // inclui as funcoes de controle do robo

// variaveis

int  minEsq = 4095, maxEsq = 0; // calibracao do sensor esquerdo
int  minDir = 4095, maxDir = 0; // calibracao do sensor direito
int  leituraEsq = 0; // valor normalizado (0 a 100)
int  leituraDir = 0; // 100 = linha preta, 0 = branco
bool roboLigado = false; // true = andando, false = parado

RockyMotors motores; // motores da rocky (A = esquerdo | B = direito)

unsigned long ultimoPrintSensores = 0; // ultima vez que mostrou os sensores no serial
unsigned long ultimoPrintMotores = 0; // ultima vez que mostrou os motores no serial

// configuracao dos pinos

void iniciarPinos() {

  // Alterado de INPUT_PULLUP para INPUT_PULLDOWN
  pinMode(PINO_BOTAO, INPUT_PULLDOWN); 
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW); // comeca apagado

  Serial.println("pinos configurados");

}

void calibrarSensores() {

  Serial.println("calibracao iniciada: passe os sensores sobre a linha e o chao");

  unsigned long inicio = millis(); // tempo inicial da calibracao

  while (millis() - inicio < TEMPO_CALIBRACAO) {

    int esq = analogRead(PINO_SENSOR_ESQ); // leitura do sensor esquerdo
    int dir = analogRead(PINO_SENSOR_DIR); // leitura do sensor direito

    if (esq < minEsq) minEsq = esq; // guarda o menor valor lido
    if (esq > maxEsq) maxEsq = esq; // guarda o maior valor lido
    if (dir < minDir) minDir = dir; // guarda o menor valor lido
    if (dir > maxDir) maxDir = dir; // guarda o maior valor lido

  }

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

}

void lerSensores() {

  int brutoEsq = analogRead(PINO_SENSOR_ESQ); // leitura do sensor esquerdo
  int brutoDir = analogRead(PINO_SENSOR_DIR);

  leituraEsq = constrain(map(brutoEsq, minEsq, maxEsq, 100, 0), 0, 100); // linha preta = 100 | branco = 0
  leituraDir = constrain(map(brutoDir, minDir, maxDir, 100, 0), 0, 100); // linha branca = 0 | preto = 100

  // transforma o valor de preto em minimo e o valor de branco em maximo pra facilitar o controle proporcional
 
  // mostra os valores no serial a cada TEMPO_SERIAL ms (se mostrar toda hora o robo fica lento)

  if (millis() - ultimoPrintSensores >= TEMPO_SERIAL) {

    ultimoPrintSensores = millis();

    Serial.print("bruto esq: ");
    Serial.print(brutoEsq);
    Serial.print(" | bruto dir: ");
    Serial.print(brutoDir);
    Serial.print(" | normal esq: ");
    Serial.print(leituraEsq);
    Serial.print(" | normal dir: ");
    Serial.println(leituraDir);

  }
}

// controle dos motores

void controlarMotores(int velEsq, int velDir) {

  // velocidade positiva = frente | negativa = re | 0 = parado
  // valores de -255 a 255

  velEsq = constrain(velEsq, -255, 255); // limita a velocidade para o intervalo permitido
  velDir = constrain(velDir, -255, 255);

  // a biblioteca da rocky usa velocidade de -100 a 100
  // entao converte de -255/255 para -100/100

  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq); // motor esquerdo (entrada A)
  motores.setSpeedRight(velDir); // motor direito (entrada B)

  // se algum motor girar pro lado errado, inverta o sinal dele -> ex: motores.setSpeedLeft(-velEsq);

}

// controle proporcional

void controleProporcional() {

  // erro > 0 = linha mais para a esquerda -> vira para a esquerda
  // erro < 0 = linha mais para a direita -> vira para a direita

  int erro = leituraEsq - leituraDir; // para inverter o sinal -> erro = leituraDir - leituraEsq
  int correcao = KP * erro; // se o robo virar pro lado errado inverta o sinal do erro

  int velEsq = VELOCIDADE_BASE - correcao;
  int velDir = VELOCIDADE_BASE + correcao;

  controlarMotores(velEsq, velDir); // manda a velocidade para os motores

  // mostra o calculo no serial a cada TEMPO_SERIAL ms

  if (millis() - ultimoPrintMotores >= TEMPO_SERIAL) {

    ultimoPrintMotores = millis();

    Serial.print("erro: ");
    Serial.print(erro);
    Serial.print(" | correcao: ");
    Serial.print(correcao);
    Serial.print(" | vel esq: ");
    Serial.print(velEsq);
    Serial.print(" | vel dir: ");
    Serial.println(velDir);

  }
}

// ligar e desligar o robo com o botao

void verificarBotao() {

  // Alterado para HIGH: agora o pino recebe energia quando o botão é pressionado
  if (digitalRead(PINO_BOTAO) == HIGH) { 

    roboLigado = !roboLigado; // inverte o estado
    digitalWrite(PINO_LED, roboLigado); // led aceso = ligado | apagado = desligado

    if (roboLigado) {
      Serial.println("botao apertado: robo LIGADO");
    } else {
      Serial.println("botao apertado: robo DESLIGADO");
    }

    delay(300); // delay para evitar ler varias vezes

    // espera soltar o botao
    // Alterado para HIGH para ele ficar preso no loop enquanto o botão estiver pressionado
    while (digitalRead(PINO_BOTAO) == HIGH) {

    }
  }
}