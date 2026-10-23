#ifndef MAPEAMENTO_H // funcao para nao incluir o mesmo arquivo mais de uma vez

// se mapeamento.h ja foi incluido ele ignora o resto do codigo

#define MAPEAMENTO_H

#include <Arduino.h>
#include <RoboCore_Rocky.h> // biblioteca da placa rocky (motores)

// sensores analogicos
// alterar caso use outro pino

const int PINO_SENSOR_ESQ = 35;
const int PINO_SENSOR_DIR = 32;
const int PINO_BOTAO = 0;
const int PINO_LED = 15; // led imbutido da placa (aceso = robo ligado)

// ajustes iniciais

const int VELOCIDADE_BASE = 150; // 0 a 255
const float KP = 1.5; // se oscilar muito, diminuir | se lento, aumentar
const int TEMPO_CALIBRACAO = 8000; // tempo pra calibrar
const int TEMPO_SERIAL = 200; // intervalo entre as mensagens no monitor serial (ms)

// variaveis

extern int minEsq, maxEsq; // calibracao do sensor esquerdo
extern int minDir, maxDir; // calibracao do sensor direito
extern int leituraEsq; // valor normalizado (0 a 100)
extern int leituraDir; // 100 = linha preta, 0 = branco
extern bool roboLigado; // true = andando, false = parado

void iniciarPinos(); // configura entradas e saidas
void calibrarSensores(); // le min e max dos sensores
void lerSensores(); // atualiza leituraEsq e leituraDir
void controlarMotores(int velEsq, int velDir); // -255 a 255
void controleProporcional(); // calcula o erro e ajusta motores
void verificarBotao(); // liga e desliga o robo

#endif // fim da parte ignorada do codigo
