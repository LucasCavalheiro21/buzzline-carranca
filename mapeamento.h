#ifndef MAPEAMENTO_H // evita incluir este arquivo duas vezes
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h> // biblioteca dos motores da rocky

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 16;
const int PINO_LED = 15;

// ajustes do movimento
// velocidades em % de forca: -100 a 100 | negativo = re

const int VEL_RETA_ESQ = 65; // reta | puxa para a direita = aumentar VEL_RETA_DIR
const int VEL_RETA_DIR = 65; // reta | puxa para a esqueda = aumentar VEL_RETA_ESQ
const int VEL_CURVA_FORA = 65; // roda de fora da curva
const int VEL_CURVA_DENTRO = -60; // roda de dentro | negativo = re | curva brusca demais = aumentar
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
