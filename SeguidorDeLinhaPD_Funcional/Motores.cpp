#include "Motores.h"
#include "Configuracoes.h"

namespace {
  //Conferir antes o comportamento HIGH/HIGH da ponte H efetivamente instalada
  void acionarCanal(uint8_t in1, uint8_t in2, int v) {
    v = constrain(v, -255, 255);
    if (v > 0) {
      digitalWrite(in2, LOW);
      analogWrite(in1, v);
    } else if (v < 0) {
      digitalWrite(in1, LOW);
      analogWrite(in2, -v);
    } else {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, HIGH);
    }
  }
}

bool Motores::movimentacaoLiberada() {
  return Config::HABILITAR_MOVIMENTACAO &&
         Config::MOTOR_ESQUERDO_INSTALADO &&
         Config::MA_IN1 != 255 && Config::MA_IN2 != 255 &&
         Config::MB_IN1 != 255 && Config::MB_IN2 != 255;
}

void Motores::iniciar() {
  //Colocar níveis baixos antes de configurar OUTPUT para evitar comando HIGH
  digitalWrite(Config::MA_IN1, LOW);
  digitalWrite(Config::MA_IN2, LOW);
  digitalWrite(Config::MB_IN1, LOW);
  digitalWrite(Config::MB_IN2, LOW);
  pinMode(Config::MA_IN1, OUTPUT);
  pinMode(Config::MA_IN2, OUTPUT);
  pinMode(Config::MB_IN1, OUTPUT);
  pinMode(Config::MB_IN2, OUTPUT);
  parar();
}

void Motores::parar() {
  digitalWrite(Config::MA_IN1, LOW);
  digitalWrite(Config::MA_IN2, LOW);
  digitalWrite(Config::MB_IN1, LOW);
  digitalWrite(Config::MB_IN2, LOW);
}

void Motores::frear() {
  if (!movimentacaoLiberada()){ 
    parar();
    return;
  }
  digitalWrite(Config::MA_IN1, HIGH);
  digitalWrite(Config::MA_IN2, HIGH);
  digitalWrite(Config::MB_IN1, HIGH);
  digitalWrite(Config::MB_IN2, HIGH);
}

void Motores::calcularComandos(float correcao, int &pwmEsquerdo, int &pwmDireito) {
  // S1 a esquerda: erro negativo -> girar a esquerda (direito mais rápido)
  pwmEsquerdo = constrain(int(Config::VELOCIDADE_BASE + correcao), 0, 255);
  pwmDireito = constrain(int(Config::VELOCIDADE_BASE - correcao), 0, 255);
}

bool Motores::aplicarComandos(int pwmEsquerdo, int pwmDireito) {
  if (!movimentacaoLiberada()) {
    parar();
    return false; 
  }
  //MONTAGEM ATUAL: A direito, B esquerdo (sempre conferir)
  acionarCanal(Config::MA_IN1, Config::MA_IN2, pwmDireito * Config::SENTIDO_DIREITO);
  acionarCanal(Config::MB_IN1, Config::MB_IN2, pwmEsquerdo * Config::SENTIDO_ESQUERDO);
  return true;
}
