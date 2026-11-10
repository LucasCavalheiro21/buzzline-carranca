#include "Sinalizacao.h"
#include "Configuracoes.h"

void Sinalizacao::iniciar() {
  pinMode(Config::LED_STATUS, OUTPUT);
  apagar();
}

void Sinalizacao::apagar() { digitalWrite(Config::LED_STATUS, LOW); }

void Sinalizacao::aguardandoSuperficie(uint8_t etapa) {
  // Ciclo de 1600 ms, pulsos de 120 ms com separacao de 140 ms.
  const unsigned long fase = millis() % 1600UL;
  const bool primeiro = fase < 120UL;
  const bool segundo = etapa == 2 && fase >= 260UL && fase < 380UL;
  digitalWrite(Config::LED_STATUS, primeiro || segundo ? HIGH : LOW);
}

void Sinalizacao::coletando() {
  // Pisca lento enquanto sao feitas as 25 leituras (~5 s).
  digitalWrite(Config::LED_STATUS, (millis() / 350UL) % 2 == 0 ? HIGH : LOW);
}

void Sinalizacao::aprovado() {
  for (uint8_t i = 0; i < 10; ++i) {
    digitalWrite(Config::LED_STATUS, HIGH);
    delay(100);
    digitalWrite(Config::LED_STATUS, LOW);
    delay(100);
  }
  apagar();
}

void Sinalizacao::reprovado() { digitalWrite(Config::LED_STATUS, HIGH); }
