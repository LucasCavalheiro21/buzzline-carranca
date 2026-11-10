#include "Botao.h"
#include "Configuracoes.h"

namespace {
  bool estaLigado = false;
  bool ultimaLeitura = HIGH;
  bool estadoEstavel = HIGH;
  unsigned long instanteMudanca = 0;
}

void Botao::iniciar() {
  pinMode(Config::BOTAO, INPUT_PULLUP);
  estaLigado = false;
  ultimaLeitura = digitalRead(Config::BOTAO);
  estadoEstavel = ultimaLeitura;
  instanteMudanca = millis();
}

void Botao::atualizar() {
  bool leitura = digitalRead(Config::BOTAO);
  if (leitura != ultimaLeitura) instanteMudanca = millis();
  if (millis() - instanteMudanca >= Config::DEBOUNCE_MS &&
      leitura != estadoEstavel) {
    estadoEstavel = leitura;
    if (estadoEstavel == LOW) {
      estaLigado = !estaLigado;
      Serial.println(estaLigado ? F("[BOTAO] ON (calculos habilitados)")
                                 : F("[BOTAO] OFF"));
    }
  }
  ultimaLeitura = leitura;
}

bool Botao::ligado() { return estaLigado; }
