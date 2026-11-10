#include "ControlePD.h"
#include "Configuracoes.h"

namespace {
  bool existeAmostraAnterior = false;
  int16_t erroAnterior = 0;
  unsigned long instanteAnteriorUs = 0;
  float derivadaFiltrada = 0.0f;
}

void ControlePD::reiniciar() {
  existeAmostraAnterior = false;
  erroAnterior = 0;
  instanteAnteriorUs = 0;
  derivadaFiltrada = 0.0f;
}

ControlePD::Resultado ControlePD::calcular(int16_t erro) {
  const unsigned long agoraUs = micros();
  Resultado resultado = {};
  resultado.proporcional = Config::KP * erro;

  if (existeAmostraAnterior) {
    // A subtracao unsigned continua funcionando apos o overflow de micros().
    const unsigned long dtUs = agoraUs - instanteAnteriorUs;
    if (dtUs > 0UL && dtUs <= Config::DT_MAXIMO_US) {
      // Converte o intervalo para segundos para calcular de/dt.
      const float dtSegundos = dtUs * 0.000001f;
      const float derivadaBruta = (erro - erroAnterior) / dtSegundos;
      // Filtro exponencial: suaviza variacoes bruscas na leitura RC.
      derivadaFiltrada += Config::ALFA_D * (derivadaBruta - derivadaFiltrada);
    } else {
      // Se o laco ficou parado, nao queremos um pico artificial no D.
      derivadaFiltrada = 0.0f;
    }
  } else {
    // Na primeira leitura valida nao existe erro anterior para derivar.
    derivadaFiltrada = 0.0f;
  }

  erroAnterior = erro;
  instanteAnteriorUs = agoraUs;
  existeAmostraAnterior = true;

  resultado.derivadaErro = derivadaFiltrada;
  resultado.derivativo = constrain(Config::KD * derivadaFiltrada,
                                  -Config::LIMITE_D, Config::LIMITE_D);
  resultado.correcao = constrain(resultado.proporcional + resultado.derivativo,
                                 -Config::LIMITE_CORRECAO,
                                  Config::LIMITE_CORRECAO);
  return resultado;
}
