#pragma once
#include <Arduino.h>

namespace Sinalizacao {
  void iniciar();
  void apagar();
  // Chamadas repetidas em espera (sem bloqueio): 1 pulso = branco; 2 = preto.
  void aguardandoSuperficie(uint8_t etapa);
  // Durante a coleta, piscar lento; chamadas repetidas.
  void coletando();
  // Sucesso: 10 pulsos rapidos, duracao de 2 segundos.
  void aprovado();
  // Falha: acender continuamente ate RESET.
  void reprovado();
}
