#ifndef MAPEAMENTO_H
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h>

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 0;
const int PINO_LED = 15;

// ajustes

const int VELOCIDADE_BASE = 195; // velocidade na reta (0 a 255)
const float KP = 1.6; // forca da curva | nao faz a curva = aumentar | samba = diminuir
const float KD = 2.25; // freio do samba 
const float KI = 0.01;
const float LIMITE_INTEGRAL = 3000; // trava da soma dos erros (anti-windup) | KI * LIMITE = maximo que o I pode corrigir

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
void controlePDI();
void verificarBotao(); // liga e desliga o robo

#endif