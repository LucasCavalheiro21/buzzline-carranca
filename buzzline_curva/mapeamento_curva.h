#ifndef MAPEAMENTO_CURVA_H // evita incluir este arquivo duas vezes
#define MAPEAMENTO_CURVA_H

#include <Arduino.h>
#include <RoboCore_Rocky.h> // biblioteca dos motores da placa Rocky

// pinos

const int PINO_SENSOR_ESQ = 35; // sensor da esquerda
const int PINO_SENSOR_DIR = 32; // sensor da direita
const int PINO_BOTAO = 16; // botao liga/desliga
const int PINO_LED = 15; // LED da placa

// controle (velocidades de -255 a 255 | negativo = re)

const int VELOCIDADE_BASE = 180; // velocidade na reta e maxima de cada roda | + = mais rapido, mas pode passar reto na curva | - = mais lento e seguro
const float KP = 1.0; // forca da correcao | + = vira mais forte (demais = samba) | - = vira menos (pouco = sai na curva)
const float KD = 1.6; // freio da correcao | + = menos samba (demais = treme) | - = correcao mais solta (pouco = samba)
const float KI = 0.01; // corrige desvio que se acumula com o tempo | + = corrige mais (demais = passa da linha depois da curva) | 0 = desliga
const float LIMITE_INTEGRAL = 3000; // teto da soma dos erros | KI x LIMITE = maximo que o I corrige | - = I mais fraco
const int VELOCIDADE_AJUSTE = 180; // velocidade em que KP, KD e KI foram ajustados | NAO mudar junto com a VELOCIDADE_BASE
const int VELOCIDADE_MINIMA = -VELOCIDADE_BASE; // menor velocidade da roda de dentro na curva | 0 = no maximo para | -40 = re leve | mais negativo = curva mais fechada e brusca

const int INTERVALO_CONTROLE = 10; // ms entre cada calculo | - = reage mais rapido | + = reage mais devagar (mudar isso muda o efeito do KD)

// sensores

const bool LINHA_BRANCA = true; // true = linha branca no chao preto | false = linha preta no chao branco
const bool PRETO_E_VALOR_ALTO = false; // se o robo fugir da linha em vez de seguir, trocar
const int CONTRASTE_MINIMO = 300; // diferenca minima entre linha e chao para a calibracao valer | - = aceita sensor mais fraco

// motores

const int SENTIDO_ESQ = -1; // se a roda esquerda girar para tras, trocar o sinal
const int SENTIDO_DIR = 1; // se a roda direita girar para tras, trocar o sinal

// tempos (ms)

const int TEMPO_CALIBRACAO = 10000; // duracao da calibracao
const int TEMPO_DEBOUNCE = 50; // tempo para o botao estabilizar | + = ignora mais tremor do botao, mas responde mais devagar

// variaveis usadas no .ino

extern bool roboLigado; // true = andando | false = parado

// funcoes (ficam no controle_curva.cpp)

void iniciarPinos(); // configura botao e LED
bool calibrarSensores(); // aprende os valores de linha e chao | true = calibrou certo
void lerSensores(); // le os dois sensores (0 a 100)
void controlarMotores(int velEsq, int velDir); // manda a velocidade de cada roda (-255 a 255)
void controlePD(); // calcula a correcao e a velocidade das rodas
void verificarBotao(); // cada aperto liga ou desliga o robo

#endif
