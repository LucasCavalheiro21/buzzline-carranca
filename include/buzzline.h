#ifndef BUZZLINE_H
#define BUZZLINE_H

#include <Arduino.h>
#include <RoboCore_Rocky.h>

// Sensores analogicos: entradas VP e VN da placa

const int PINO_SENSOR_ESQ = 36;   // VP
const int PINO_SENSOR_DIR = 39;   // VN
const int PINO_BOTAO = 0;

// LED "L" da placa (GPIO15) e controlado pela classe RockyLED.

const int   VELOCIDADE_BASE = 60; 
const float KP = 0.6;  
const int   TEMPO_CALIBRACAO = 4000;  

const bool INVERTER_MOTOR_ESQ = false;
const bool INVERTER_MOTOR_DIR = false;

extern RockyMotors motores;
extern RockyLED led;

extern int  minEsq, maxEsq; // calibracao do sensor esquerdo
extern int  minDir, maxDir; // calibracao do sensor direito

extern int  leituraEsq; // valor normalizado (0 a 100)
extern int  leituraDir; // 100 = linha preta, 0 = branco

extern bool roboLigado; // true = andando, false = parado

void iniciarPinos(); // configura entradas, ADC e LED
void calibrarSensores(); // le min e max dos sensores
void lerSensores(); // atualiza leituraEsq e leituraDir
void controlarMotores(int velEsq, int velDir); // -100 a 100
void controleProporcional(); // calcula o erro e ajusta motores
void verificarBotao(); // liga/desliga com o botao

#endif