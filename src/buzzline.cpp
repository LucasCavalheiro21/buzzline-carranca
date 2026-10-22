#include "buzzline.h"

// ---------- Objetos da placa Rocky ----------
RockyMotors motores;
RockyLED    led;

// ---------- Definicao das variaveis globais ----------
// O ADC do ESP32 e de 12 bits (0 a 4095), e nao 0 a 1023 como no Arduino UNO
int  minEsq = 4095, maxEsq = 0;
int  minDir = 4095, maxDir = 0;

int  leituraEsq = 0;
int  leituraDir = 0;

bool robotLigado = false;

// ------------------------------------------------------------
// Configura os pinos
// ------------------------------------------------------------
void iniciarPinos() {
  // Motores: ja configurados pela biblioteca (objeto "motores")
  motores.stop();

  // Sensores analogicos (12 bits, faixa ate ~3,3 V)
  analogReadResolution(12);
  analogSetPinAttenuation(PINO_SENSOR_ESQ, ADC_11db);
  analogSetPinAttenuation(PINO_SENSOR_DIR, ADC_11db);

  // Botao BOOT: solto = HIGH, apertado = LOW
  pinMode(PINO_BOTAO, INPUT_PULLUP);

  led.off();
}

// ------------------------------------------------------------
// Calibracao: durante alguns segundos, mova o robo (ou passe os
// sensores) sobre a linha preta E sobre o chao branco.
// O codigo guarda o menor e o maior valor lido de cada sensor.
// O LED pisca rapido enquanto calibra.
// ------------------------------------------------------------
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

  // Evita divisao por zero no map() se um sensor nao variou
  if (maxEsq <= minEsq) maxEsq = minEsq + 1;
  if (maxDir <= minDir) maxDir = minDir + 1;

  led.off();

  Serial.printf("Calibracao  ESQ: %d a %d  |  DIR: %d a %d\n",
                minEsq, maxEsq, minDir, maxDir);
}

// ------------------------------------------------------------
// Leitura: converte cada sensor para uma escala de 0 a 100
//   100 = preto (linha)   |   0 = branco
// (preto reflete menos luz, entao o valor bruto e MENOR no preto;
//  por isso a escala e invertida no map)
// ------------------------------------------------------------
void lerSensores() {
  int brutoEsq = analogRead(PINO_SENSOR_ESQ);
  int brutoDir = analogRead(PINO_SENSOR_DIR);

  leituraEsq = map(brutoEsq, minEsq, maxEsq, 100, 0);
  leituraDir = map(brutoDir, minDir, maxDir, 100, 0);

  // Garante que fica entre 0 e 100
  leituraEsq = constrain(leituraEsq, 0, 100);
  leituraDir = constrain(leituraDir, 0, 100);
}

// ------------------------------------------------------------
// Controle dos motores (usa a biblioteca da Rocky)
//   velocidade positiva = frente | negativa = re | 0 = parado
//   valores de -100 a 100 (%)
// ------------------------------------------------------------
void controlarMotores(int velEsq, int velDir) {
  velEsq = constrain(velEsq, -100, 100);
  velDir = constrain(velDir, -100, 100);

  if (INVERTER_MOTOR_ESQ) velEsq = -velEsq;
  if (INVERTER_MOTOR_DIR) velDir = -velDir;

  if (velEsq == 0 && velDir == 0) {
    motores.stop();
    return;
  }

  // A biblioteca ja troca o sentido sozinha quando o valor e negativo
  motores.setSpeedLeft(velEsq);    // motor A
  motores.setSpeedRight(velDir);   // motor B
}

// ------------------------------------------------------------
// Controle proporcional
//   erro = esquerdo - direito
//   erro > 0 -> linha mais para a esquerda -> vira para a esquerda
//   erro < 0 -> linha mais para a direita  -> vira para a direita
// ------------------------------------------------------------
void controleProporcional() {
  int erro = leituraEsq - leituraDir;
  int correcao = KP * erro;

  int velEsq = VELOCIDADE_BASE - correcao;
  int velDir = VELOCIDADE_BASE + correcao;

  controlarMotores(velEsq, velDir);
}

// ------------------------------------------------------------
// Start/Stop: cada vez que apertar o botao BOOT, alterna
// ligado/desligado. LED aceso = robo andando.
// ------------------------------------------------------------
void verificarBotao() {
  if (digitalRead(PINO_BOTAO) == LOW) {   // botao apertado
    robotLigado = !robotLigado;           // inverte o estado
    controlarMotores(0, 0);               // para enquanto o botao esta apertado

    if (robotLigado) led.on();
    else             led.off();

    delay(300);                           // evita ler varias vezes o mesmo aperto

    // Espera soltar o botao
    while (digitalRead(PINO_BOTAO) == LOW) {
      delay(10);
    }
  }
}