#include "buzzline.h"

RockyMotors motores;
RockyLED    led;

int  minEsq = 4095, maxEsq = 0;
int  minDir = 4095, maxDir = 0;

int  leituraEsq = 0;
int  leituraDir = 0;

bool roboLigado = false;

void iniciarPinos() {

  motores.stop();

  analogReadResolution(12);
  analogSetPinAttenuation(PINO_SENSOR_ESQ, ADC_11db);
  analogSetPinAttenuation(PINO_SENSOR_DIR, ADC_11db);

  pinMode(PINO_BOTAO, INPUT_PULLUP);

  led.off();

}

void calibrarSensores() {

  unsigned long inicio = millis();
  led.blink(100);

  while (millis() - inicio < TEMPO_CALIBRACAO) {

    int esq = analogRead(PINO_SENSOR_ESQ);
    int dir = analogRead(PINO_SENSOR_DIR);

    if (esq < minEsq) minEsq = esq;
    if (esq > maxEsq) maxEsq = esq;
    if (dir < minDir) minDir = dir;
    if (dir > maxDir) maxDir = dir;

    led.update();
    delay(2);

  }

  if (maxEsq <= minEsq) maxEsq = minEsq + 1;
  if (maxDir <= minDir) maxDir = minDir + 1;

  led.off();

  Serial.printf("Calibracao  ESQ: %d a %d  |  DIR: %d a %d\n", minEsq, maxEsq, minDir, maxDir);

}

void lerSensores() {

  int brutoEsq = analogRead(PINO_SENSOR_ESQ);
  int brutoDir = analogRead(PINO_SENSOR_DIR);

  leituraEsq = map(brutoEsq, minEsq, maxEsq, 0, 100);
  leituraDir = map(brutoDir, minDir, maxDir, 0, 100); // inverter para 100 -> 0 se a linha for preta

  leituraEsq = constrain(leituraEsq, 0, 100);
  leituraDir = constrain(leituraDir, 0, 100);

}

void controlarMotores(int velEsq, int velDir) {

  velEsq = constrain(velEsq, -100, 100);
  velDir = constrain(velDir, -100, 100);

  if (INVERTER_MOTOR_ESQ) velEsq = -velEsq;
  if (INVERTER_MOTOR_DIR) velDir = -velDir;

  if (velEsq == 0 && velDir == 0) {

    motores.stop();
    return;

  }

  motores.setSpeedLeft(velEsq); // motor A
  motores.setSpeedRight(velDir); // motor B

}

void controleProporcional() {

  int erro = leituraEsq - leituraDir;
  int correcao = KP * erro;

  int velEsq = VELOCIDADE_BASE - correcao;
  int velDir = VELOCIDADE_BASE + correcao;

  controlarMotores(velEsq, velDir);

}

void verificarBotao() {

  if (digitalRead(PINO_BOTAO) == LOW) {

    roboLigado = !roboLigado;
    controlarMotores(0, 0);

    if (roboLigado) led.on();
    else led.off();

    delay(300);

    while (digitalRead(PINO_BOTAO) == LOW) {
      delay(10);
      
    }
  }
}