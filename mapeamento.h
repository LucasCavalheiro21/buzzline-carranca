#ifndef MAPEAMENTO_H // evita incluir este arquivo duas vezes
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h> // biblioteca dos motores da rocky

/* código simplificado (sem PID)

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 16;
const int PINO_LED = 15;

// ajustes do movimento
// velocidades em % de forca: -100 a 100 | negativo = re

const int VEL_RETA_ESQ = 70; // reta | puxa para a direita = aumentar VEL_RETA_DIR
const int VEL_RETA_DIR = 70; // reta | puxa para a esqueda = aumentar VEL_RETA_ESQ
const int VEL_CURVA_FORA = 70; // roda de fora da curva
const int VEL_CURVA_DENTRO = -35; // roda de dentro | negativo = re | curva brusca demais = aumentar
const int ZONA_MORTA = 500; // diferenca entre sensores que ainda conta como reto | corrige a toa = aumentar
const int TEMPO_MEMORIA = 100; // ms que continua virando apos perder a linha | passa do ponto = diminuir

// sensores

const bool LINHA_BRANCA = true; // true = linha branca no chao preto | false = linha preta no chao branco

// motores

const int SENTIDO_ESQ = -1; // roda girando para tras = trocar o sinal
const int SENTIDO_DIR = 1;

// tempos (ms)

const int TEMPO_DEBOUNCE = 50; // tempo para o botao estabilizar

// variavel usada no .ino

extern bool roboLigado; // true = andando | false = parado

// funcoes (estao no controle.cpp)

void iniciarPinos();
void lerSensores();
void controlarMotores(int velEsq, int velDir); // -100 a 100 | negativo = re
void seguirLinha();
void verificarBotao(); // liga e desliga o robo

#endif

*/

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 16;
const int PINO_LED = 15;

// ajustes

const int VELOCIDADE_BASE = 150; // velocidade na reta (0 a 255)
const int VELOCIDADE_MINIMA = 0; // menor velocidade de uma roda na curva | negativo = deixa a roda dar re
const float KP = 0.6; // forca da curva | nao faz a curva = aumentar | samba = diminuir
const float KD = 0.0; // freio do samba | subir de 2 em 2
const int INTERVALO_CONTROLE = 10; // ms entre cada calculo do controle

// sensores

const bool LINHA_BRANCA = true; // true = linha branca no chao preto | false = linha preta no chao branco
const bool PRETO_E_VALOR_ALTO = false; // se o robo fugir da linha em vez de seguir, trocar
const int CONTRASTE_MINIMO = 300; // diferenca minima entre preto e branco para a calibracao valer

// motores

const int SENTIDO_ESQ = -1; // se a roda girar para tras, trocar o sinal
const int SENTIDO_DIR = 1;

// tempos (ms)

const int TEMPO_CALIBRACAO = 10000;
const int TEMPO_DEBOUNCE = 50; // tempo para o botao estabilizar

// variaveis

extern bool roboLigado; // true = andando | false = parado

// funcoes

void iniciarPinos();
bool calibrarSensores(); // true = calibrou certo
void lerSensores();
void controlarMotores(int velEsq, int velDir); // -255 a 255 | negativo = re
void controlePD();
void verificarBotao(); // liga e desliga o robo

#endif
