#ifndef MAPEAMENTO_H
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h>

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 16;
const int PINO_LED = 15;

// controle PD (velocidades de -255 a 255)

const int VELOCIDADE_BASE = 150; // velocidade na reta
const int VELOCIDADE_MINIMA = -90; // menor velocidade de uma roda na curva (negativo = re)
const float KP = 0.6; // nao faz a curva = aumentar | samba = diminuir
const float KD = 1.6; // freio do samba
const float KI = 0.0; // 0 = desligado
const float LIMITE_INTEGRAL = 3000; // anti-windup

const int INTERVALO_CONTROLE = 10; // ms entre cada calculo

// curvas fechadas

const int VELOCIDADE_BASE_CURVA = 110; // velocidade base com erro maximo | igual a VELOCIDADE_BASE = sem reducao

const int LIMIAR_LINHA = 25; // leitura >= isso = sensor ve a linha (0 a 100)
const int LIMIAR_LADO = 60; // leitura >= isso = guarda o lado da linha | gira a toa na reta = aumentar | perde a curva = diminuir
const unsigned long MEMORIA_LADO = 300; // ms que o lado guardado vale
const int LIMIAR_CRUZAMENTO = 70; // os dois sensores >= isso = cruzamento | bem acima do LIMIAR_LINHA para a borda da linha nao contar
const unsigned long JANELA_CRUZAMENTO = 150; // linha forte num sensor e depois no outro dentro desse tempo = cruzamento torto
const unsigned long TEMPO_CRUZAMENTO = 200; // ms sem guardar lado depois de um cruzamento
const unsigned long TEMPO_MAX_CRUZAMENTO = 400; // ms com os dois sensores na linha antes de parar (fora da pista)

const int VELOCIDADE_BUSCA_FORA = 150; // roda de fora da curva
const int VELOCIDADE_BUSCA_DENTRO = -90; // roda de dentro da curva
const unsigned long TEMPO_MAX_BUSCA = 1200; // ms procurando a linha antes de parar

// sensores

const bool LINHA_BRANCA = true; // true = linha branca no chao preto
const bool PRETO_E_VALOR_ALTO = false; // se o robo fugir da linha, trocar
const int CONTRASTE_MINIMO = 300; // diferenca minima entre linha e chao na calibracao
const int AMOSTRAS_SENSOR = 4; // leituras por media

// motores

const int SENTIDO_ESQ = -1; // se a roda girar para tras, trocar o sinal
const int SENTIDO_DIR = 1;

// tempos (ms)

const int TEMPO_CALIBRACAO = 10000;
const int TEMPO_DEBOUNCE = 50;

// variaveis

extern bool roboLigado;

// funcoes

void iniciarPinos();
bool calibrarSensores(); // true = calibrou certo
void lerSensores();
void controlarMotores(int velEsq, int velDir); // -255 a 255
void seguirLinha();
void verificarBotao();

#endif