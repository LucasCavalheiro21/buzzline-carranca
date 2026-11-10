#pragma once
#include <Arduino.h>

namespace Config {
  //Sensores RC, na ordem dos conectores S1 - S7
  constexpr unsigned long SERIAL_BAUD = 115200UL; //Diagnóstico sem atrasar o controlador
  constexpr uint8_t NUM_SENSORES = 7;
  constexpr uint8_t PINOS_SENSORES[NUM_SENSORES] = {A0, A1, A2, A3, A4, A5, 4};
  constexpr uint16_t TEMPO_LIMITE_RC_US = 2500;

  //Botão ligado entre D12 e GND
  constexpr uint8_t BOTAO = 12;
  constexpr unsigned long DEBOUNCE_MS = 40;

  //Mini ponte H de quatro entradas (L298N)
  // NOVA MONTAGEM: canal A = DIREITO; canal B = ESQUERDO
  constexpr uint8_t MA_IN1 = 5;
  constexpr uint8_t MA_IN2 = 6;
  constexpr uint8_t MB_IN1 = 10;
  constexpr uint8_t MB_IN2 = 9;
  constexpr int8_t SENTIDO_ESQUERDO = -1; //De acordo com o teste dos motores: comando elétrico positivo = trás
  constexpr int8_t SENTIDO_DIREITO = -1;  //De acordo com o teste dos motores: comando elétrico positivo = trás

  constexpr uint8_t LED_STATUS = LED_BUILTIN; // D13, LED interno do Arduino Uno (auxiliar)
  constexpr bool MOTOR_ESQUERDO_INSTALADO = true;
  //Motores testados individualmente e em conjunto; TESTAR primeiro com rodas suspensas
  constexpr bool HABILITAR_MOVIMENTACAO = true; // APENAS depois do teste com rodas suspensas

  constexpr int VELOCIDADE_BASE = 100; // PWM reduzido para teste (faixa valida 0 - 255)
  constexpr float KP = 0.015f;        // Ganho proporcional: PWM por unidade de erro
  constexpr float KD = 0.0002f;       // Ganho derivativo: PWM / (erro por segundo)
  constexpr float ALFA_D = 0.25f;     // Filtro passa-baixas na derivada (0 - 1)
  constexpr float LIMITE_D = 35.0f;   // Limite independente da parcela derivativa
  constexpr float LIMITE_CORRECAO = 80.0f; // Limite da correção total
  constexpr unsigned long DT_MAXIMO_US = 200000UL; // Reiniciar D se ocorrer uma pausa grande
  constexpr bool INVERTER_POSICAO = false; // true se S1 estiver fisicamente a direita

  //CALIBRAÇÃO: 25 amostras a cada ~200 ms em cada superficie
  constexpr uint8_t N_AMOSTRAS = 25;
  constexpr uint8_t DESCARTAR_EXTREMOS = 5;
  constexpr unsigned long INTERVALO_AMOSTRAS_MS = 200;
  constexpr uint16_t CONTRASTE_MINIMO_US = 120;
  //Para calibrar TODOS os sensores em preto uniforme e bem escuro
  // Se a superfície preta real não atingir este mínimo mesmo com distância correta, refazer teste e/ou investigar a montagem antes de ajustar este critério
  constexpr uint16_t PRETO_MINIMO_US = 1500;

  //Heuristicas iniciais  (precisam ser validadas na pista)
  constexpr uint16_t MINIMO_PRETO_PARA_LINHA = 250;  // normalizado entre 0 e 1000
  constexpr uint16_t PRETO_FAIXA_LARGA = 800;
  constexpr uint8_t MIN_SENSORES_FAIXA_LARGA = 6;
  constexpr unsigned long INTERVALO_DEBUG_MS = 250;
}