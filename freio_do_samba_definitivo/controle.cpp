#include "mapeamento.h" // pinos, ajustes e funcoes

// sensores

int leituraEsq = 0; int leituraDir = 0; // leitura de cada sensor | 0 = chao | 100 = linha
int minEsq = 4095; int maxEsq = 0; // menor e maior valor do sensor esquerdo na calibracao
int minDir = 4095; int maxDir = 0; // menor e maior valor do sensor direito na calibracao

// partes da correcao (calculadas no controlePDI)

float P; // parte que corrige o erro de agora
float D; // parte que freia se o erro muda rapido
float I; // parte que corrige o erro acumulado
bool roboLigado = false; // true = andando | false = parado

RockyMotors motores; // motores da placa Rocky

// controle

int erroAnterior = 0; // erro do calculo anterior (usado no D)
unsigned long ultimoControle = 0; // quando foi o ultimo calculo
float integralErro = 0; // soma dos erros (usado no I)

// botao

bool ultimaLeituraBotao = HIGH; // leitura anterior do botao
bool estadoBotao = HIGH; // estado confirmado do botao | HIGH = solto | LOW = apertado
unsigned long instanteMudancaBotao = 0; // quando a leitura do botao mudou

// configura botao e LED

void iniciarPinos(){

  pinMode(PINO_BOTAO, INPUT); // botao como entrada
  pinMode(PINO_LED, OUTPUT); // LED como saida
  digitalWrite(PINO_LED, LOW); // comeca apagado

}

// le os sensores por TEMPO_CALIBRACAO e guarda o menor e o maior valor de cada um

bool calibrarSensores(){

  unsigned long inicio = millis(); // quando a calibracao comecou
  bool sinalDisparado = false; // true = ja passou da metade

  while (millis() - inicio < TEMPO_CALIBRACAO){

    unsigned long tempoDecorrido = millis() - inicio;

    // na metade: LED aceso por 1 s para mover o robo

    if (tempoDecorrido >= 5000 && !sinalDisparado) {

      digitalWrite(PINO_LED, HIGH);
      delay(1000);

      inicio += 1000; // desconta a pausa do tempo da calibracao
      sinalDisparado = true;
      continue;

    }

    int esq = analogRead(PINO_SENSOR_ESQ); // leitura bruta (0 a 4095)
    int dir = analogRead(PINO_SENSOR_DIR);

    // guarda o menor e o maior valor de cada sensor

    minEsq = min(minEsq, esq);
    maxEsq = max(maxEsq, esq);
    minDir = min(minDir, dir);
    maxDir = max(maxDir, dir);

    if (!sinalDisparado) {

      digitalWrite(PINO_LED, (millis() / 250) % 2); // primeira metade: pisca normal

    } else {

      digitalWrite(PINO_LED, (millis() / 125) % 2); // segunda metade: pisca rapido

    }

    delay(2);

  }

  digitalWrite(PINO_LED, LOW); // apaga o LED no fim

  // true se os dois sensores viram diferenca suficiente entre linha e chao

  return (maxEsq - minEsq >= CONTRASTE_MINIMO) && (maxDir - minDir >= CONTRASTE_MINIMO);

}

// converte a leitura bruta para 0 a 100 (0 = chao | 100 = linha)

int normalizar(int bruto, int minimo, int maximo){

  int valor = constrain(map(bruto, minimo, maximo, 0, 100), 0, 100); // bruto -> 0 a 100

  // inverte se precisar, para 100 = preto

  if (!PRETO_E_VALOR_ALTO){

    valor = 100 - valor;

  }

  // inverte se a linha for branca, para 100 = linha

  if (LINHA_BRANCA){

    valor = 100 - valor;

  }

  return valor;

}

// le os dois sensores ja convertidos para 0 a 100

void lerSensores(){

  leituraEsq = normalizar(analogRead(PINO_SENSOR_ESQ), minEsq, maxEsq);
  leituraDir = normalizar(analogRead(PINO_SENSOR_DIR), minDir, maxDir);

}

// cada aperto liga ou desliga o robo

void verificarBotao(){

  bool leitura = digitalRead(PINO_BOTAO); // HIGH = solto | LOW = apertado

  // a leitura mudou: comeca a contar o tempo

  if (leitura != ultimaLeituraBotao){

    instanteMudancaBotao = millis();

  }

  // so aceita a mudanca se ficou estavel por TEMPO_DEBOUNCE (ignora o tremor do botao)

  if (millis() - instanteMudancaBotao >= TEMPO_DEBOUNCE && leitura != estadoBotao){

    estadoBotao = leitura;

    if (estadoBotao == LOW) { // botao apertado: liga ou desliga

      roboLigado = !roboLigado;
      digitalWrite(PINO_LED, roboLigado); // LED aceso = ligado
      erroAnterior = 0; // zera o D
      integralErro = 0; // zera o I

    }
  }

  ultimaLeituraBotao = leitura;

}

// manda a velocidade de cada roda (-255 a 255 | negativo = re)

void controlarMotores(int velEsq, int velDir){

  // converte para a escala da biblioteca da Rocky (-100 a 100)

  velEsq = map(velEsq, -255, 255, -100, 100);
  velDir = map(velDir, -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);

}

// calcula a correcao e a velocidade das rodas

void controlePDI(){

  // so calcula a cada INTERVALO_CONTROLE ms

  if (millis() - ultimoControle < INTERVALO_CONTROLE){

    return;

  }

  ultimoControle = millis();

  // erro: positivo = linha a esquerda | negativo = linha a direita | 0 = centralizado

  int erro = leituraEsq - leituraDir;

  integralErro = constrain(integralErro + erro, -LIMITE_INTEGRAL, LIMITE_INTEGRAL); // soma os erros, com teto
  P = KP * erro; // corrige o erro de agora
  D = KD * (erro - erroAnterior); // freia se o erro estiver mudando rapido
  I = KI * integralErro; // corrige o erro acumulado

  int correcao = P + I + D; // quanto a roda de dentro desacelera

  erroAnterior = erro;

  // roda de fora fica na base | roda de dentro desacelera (pode parar ou dar re)
  // cada roda fica entre -VELOCIDADE_BASE e VELOCIDADE_BASE

  int velEsq = constrain(VELOCIDADE_BASE - correcao, -VELOCIDADE_BASE, VELOCIDADE_BASE);
  int velDir = constrain(VELOCIDADE_BASE + correcao, -VELOCIDADE_BASE, VELOCIDADE_BASE);

  controlarMotores(velEsq, velDir);
  
}
