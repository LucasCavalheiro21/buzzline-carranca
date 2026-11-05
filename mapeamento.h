#ifndef MAPEAMENTO_H
#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h>

// pinos

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 0; // botao BOOT da placa
const int PINO_LED = 15;

// ajustes

const int VELOCIDADE_BASE = 100; // 0 a 255. comece baixo e so aumente quando ele seguir bem
const int VELOCIDADE_MINIMA = 0; // 0 = a roda de dentro da curva so para | negativo = a roda da re | subir de 10 em 10
const float KP = 1.0; // se nao faz a curva, aumentar | se sambar, diminuir
const float KD = 0.0; // subir de 2 em 2
const int INTERVALO_CONTROLE = 10; // tempo entre cada calculo do controle 

// sensores

const bool LINHA_BRANCA = true; // true = linha BRANCA em chao PRETO | false = linha PRETA em chao BRANCO
const bool PRETO_E_VALOR_ALTO = true; // true se o valor BRUTO do sensor SOBE quando ele esta em cima do preto | se diminuir, false
const int CONTRASTE_MINIMO = 300; // diferenca minima entre max e min na calibracao (0 a 4095)

// motores

const int SENTIDO_ESQ = -1; // inverter sinal se girar ao contrario
const int SENTIDO_DIR = 1;

// tempo

const int TEMPO_CALIBRACAO = 8000; // tempo para calibrar 
const int TEMPO_SERIAL = 200; // intervalo entre mensagens 
const int TEMPO_DEBOUNCE = 50; // tempo do botao

// variaveis

extern int minEsq, maxEsq; // calibracao do sensor esquerdo
extern int minDir, maxDir; // calibracao do sensor direito
extern int leituraEsq; // valor normalizado (0 a 100)
extern int leituraDir; // 100 = em cima da LINHA | 0 = em cima do CHAO
extern bool roboLigado; // true = andando, false = parado

// funcoes

void iniciarPinos(); // configura entradas e saidas
bool calibrarSensores(); // le min e max dos sensores (false = falhou)
void lerSensores(); // atualiza leituraEsq e leituraDir
void controlarMotores(int velEsq, int velDir); // -255 a 255
void controlePD(); // calcula o erro e ajusta motores
void reiniciarControle(); // zera a memoria do controle
void verificarBotao(); // liga e desliga o robo

#endif
