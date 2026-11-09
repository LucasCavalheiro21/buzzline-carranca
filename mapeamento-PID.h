#ifndef MAPEAMENTO_H // evita incluir este arquivo duas vezes
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h> // biblioteca dos motores da rocky

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 16;
const int PINO_LED = 15;

// ajustes

const int VELOCIDADE_BASE = 150; // velocidade na reta (0 a 255)
const int VELOCIDADE_MINIMA = 0; // menor velocidade de uma roda na curva | negativo = deixa a roda dar re
const float KP = 0.6; // forca da curva | nao faz a curva = aumentar | samba = diminuir
const float KI = 0.0; // continua virando depois que a linha escapa | subir de 0.01 em 0.01 | samba na saida da curva = diminuir
const float KD = 0.0; // freio do samba | subir de 0.5 em 0.5
const int LIMITE_INTEGRAL = 3000; // maior valor que a soma do I pode ter (evita virar demais)
const float ESQUECIMENTO_I = 0.97; // quanto da soma fica a cada calculo | vira tempo demais = diminuir (ex: 0.95)
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
void controlePID();
void verificarBotao(); // liga e desliga o robo

#endif
