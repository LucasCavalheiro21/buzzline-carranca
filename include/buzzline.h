#ifndef BUZZLINE_H
#define BUZZLINE_H

#include <Arduino.h>
#include <RoboCore_Rocky.h>

// ------------------------------------------------------------
// PINOS - RoboCore BlackBoard Rocky (ESP32)
// ------------------------------------------------------------
// Sensores analogicos: entradas VP e VN da placa
//   VP = GPIO36 | VN = GPIO39  (so entrada, ADC1, funciona com Wi-Fi ligado)
//   ATENCAO: o sinal do sensor deve ser no maximo 3,3 V!
const int PINO_SENSOR_ESQ = 36;   // VP
const int PINO_SENSOR_DIR = 39;   // VN

// Motores: NAO precisa definir pinos, a biblioteca ja cuida disso.
//   Motor A (GPIO25/26) = esquerdo
//   Motor B (GPIO14/13) = direito
//   (GPIO33 = sleep dos drivers, GPIO4/27 = motor C, nao usados aqui)

// Botao de start/stop: o proprio botao BOOT da placa (GPIO0).
//   Nao precisa ligar nada. Solto = HIGH, apertado = LOW.
const int PINO_BOTAO = 0;

// LED "L" da placa (GPIO15) e controlado pela classe RockyLED.

// ------------------------------------------------------------
// AJUSTES DO ROBO (mexa aqui para afinar o comportamento)
// ------------------------------------------------------------
// Na Rocky a velocidade vai de -100 a 100 (%), e nao de -255 a 255.
const int   VELOCIDADE_BASE  = 60;    // 0 a 100 (%)  (~150 de 255)
const float KP               = 0.6;   // ganho proporcional (1.5 * 100/255)
const int   TEMPO_CALIBRACAO = 4000;  // ms para calibrar

// Se um motor girar para tras quando deveria ir para frente,
// troque o valor correspondente para true (ou inverta os fios).
const bool INVERTER_MOTOR_ESQ = false;
const bool INVERTER_MOTOR_DIR = false;

// ------------------------------------------------------------
// OBJETOS DA PLACA (definidos no .cpp)
// ------------------------------------------------------------
extern RockyMotors motores;   // pontes H da Rocky
extern RockyLED    led;       // LED da placa

// ------------------------------------------------------------
// VARIAVEIS GLOBAIS (definidas no .cpp)
// ------------------------------------------------------------
extern int  minEsq, maxEsq;   // calibracao do sensor esquerdo
extern int  minDir, maxDir;   // calibracao do sensor direito

extern int  leituraEsq;       // valor normalizado (0 a 100)
extern int  leituraDir;       // 100 = linha preta, 0 = branco

extern bool robotLigado;      // true = andando, false = parado

// ------------------------------------------------------------
// FUNCOES (definidas no .cpp)
// ------------------------------------------------------------
void iniciarPinos();          // configura entradas, ADC e LED
void calibrarSensores();      // le min e max dos sensores
void lerSensores();           // atualiza leituraEsq e leituraDir
void controlarMotores(int velEsq, int velDir); // -100 a 100
void controleProporcional();  // calcula o erro e ajusta motores
void verificarBotao();        // liga/desliga com o botao

#endif