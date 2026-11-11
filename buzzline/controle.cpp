#include "mapeamento.h"

// sensores (0 a 100 | 100 = linha)

int leituraEsq = 0;
int minEsq = 4095;
int maxEsq = 0;

int leituraDir = 0;
int minDir = 4095;
int maxDir = 0;

bool roboLigado = false;

RockyMotors motores;

// controle PD

int erroAnterior = 0;
bool primeiroCalculo = true;
float integralErro = 0;
unsigned long ultimoControle = 0;

// lado da linha (mesmo sinal do erro)

const int LADO_NENHUM = 0;
const int LADO_ESQUERDA = 1;
const int LADO_DIREITA = -1;

int ultimoLado = LADO_NENHUM;
unsigned long instanteUltimoLado = 0;

bool buscandoLinha = false;
int ladoBusca = LADO_NENHUM;
unsigned long inicioBusca = 0;

bool noCruzamento = false;
unsigned long inicioCruzamento = 0;
unsigned long instanteCruzamento = 0;

// botao

bool ultimaLeituraBotao = HIGH;
bool estadoBotao = HIGH;
unsigned long instanteMudancaBotao = 0;

void iniciarPinos(){

  pinMode(PINO_BOTAO, INPUT_PULLUP); // apertado = LOW
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);

}

// media de leituras (reduz o ruido do ADC)

int lerMedia(int pino){

  long soma = 0;

  for (int i = 0; i < AMOSTRAS_SENSOR; i++){
    soma += analogRead(pino);
  }

  return soma / AMOSTRAS_SENSOR;

}

// le os sensores por um tempo guardando o menor e o maior valor de cada um

void lerExtremos(unsigned long duracao, int piscada){

  unsigned long inicio = millis();

  while (millis() - inicio < duracao){

    int esq = lerMedia(PINO_SENSOR_ESQ);
    int dir = lerMedia(PINO_SENSOR_DIR);

    minEsq = min(minEsq, esq);
    maxEsq = max(maxEsq, esq);
    minDir = min(minDir, dir);
    maxDir = max(maxDir, dir);

    digitalWrite(PINO_LED, (millis() / piscada) % 2);

    delay(2);

  }

}

bool calibrarSensores(){

  lerExtremos(TEMPO_CALIBRACAO / 2, 250);

  // na metade: led aceso por 1 s para mover o robo
  digitalWrite(PINO_LED, HIGH);
  delay(1000);

  lerExtremos(TEMPO_CALIBRACAO / 2, 125); // segunda metade: pisca mais rapido

  digitalWrite(PINO_LED, LOW);

  return (maxEsq - minEsq >= CONTRASTE_MINIMO) && (maxDir - minDir >= CONTRASTE_MINIMO);

}

// converte o valor bruto para 0 a 100 (100 = linha)

int normalizar(int bruto, int minimo, int maximo){

  int valor = constrain(map(bruto, minimo, maximo, 0, 100), 0, 100);

  if (!PRETO_E_VALOR_ALTO){
    valor = 100 - valor;
  }

  if (LINHA_BRANCA){
    valor = 100 - valor;
  }

  return valor;

}

void lerSensores(){

  leituraEsq = normalizar(lerMedia(PINO_SENSOR_ESQ), minEsq, maxEsq);
  leituraDir = normalizar(lerMedia(PINO_SENSOR_DIR), minDir, maxDir);

}

void reiniciarControle(){

  primeiroCalculo = true;
  integralErro = 0;
  ultimoLado = LADO_NENHUM;
  buscandoLinha = false;
  noCruzamento = false;

}

void desligarRobo(){

  roboLigado = false;
  digitalWrite(PINO_LED, LOW);
  controlarMotores(0, 0);
  reiniciarControle();

}

// cada aperto liga ou desliga o robo

void verificarBotao(){
  bool leitura = digitalRead(PINO_BOTAO);

  if (leitura != ultimaLeituraBotao){

    instanteMudancaBotao = millis();

  }

  // debounce

  if (millis() - instanteMudancaBotao >= TEMPO_DEBOUNCE && leitura != estadoBotao){

    estadoBotao = leitura;

    if (estadoBotao == LOW) {

      roboLigado = !roboLigado;
      digitalWrite(PINO_LED, roboLigado);
      reiniciarControle();

    }
  }

  ultimaLeituraBotao = leitura;

}

// a biblioteca da rocky usa -100 a 100

void controlarMotores(int velEsq, int velDir){

  velEsq = map(constrain(velEsq, -255, 255), -255, 255, -100, 100);
  velDir = map(constrain(velDir, -255, 255), -255, 255, -100, 100);

  motores.setSpeedLeft(velEsq * SENTIDO_ESQ);
  motores.setSpeedRight(velDir * SENTIDO_DIR);
}

// gira no eixo para o lado em que a linha estava

void girarParaLado(int lado){

  if (lado == LADO_ESQUERDA){
    controlarMotores(VELOCIDADE_BUSCA_DENTRO, VELOCIDADE_BUSCA_FORA);
  } else {
    controlarMotores(VELOCIDADE_BUSCA_FORA, VELOCIDADE_BUSCA_DENTRO);
  }

}

void controlePID(){

  // positivo = linha a esquerda | negativo = linha a direita
  int erro = leituraEsq - leituraDir;

  if (primeiroCalculo){
    erroAnterior = erro; // evita tranco do D
    primeiroCalculo = false;
  }

  integralErro = constrain(integralErro + erro, -LIMITE_INTEGRAL, LIMITE_INTEGRAL);

  float P = KP * erro;
  float I = KI * integralErro;
  float D = KD * (erro - erroAnterior);
  int correcao = P + I + D;

  erroAnterior = erro;

  // reduz a velocidade na curva
  int velBase = map(abs(erro), 0, 100, VELOCIDADE_BASE, VELOCIDADE_BASE_CURVA);

  int velEsq = constrain(velBase - correcao, VELOCIDADE_MINIMA, 255);
  int velDir = constrain(velBase + correcao, VELOCIDADE_MINIMA, 255);

  controlarMotores(velEsq, velDir);
}

// a linha fica entre os sensores: os dois no chao pode ser "centralizado" ou "perdido na curva"
// o lado guardado diferencia os dois casos

void seguirLinha(){

  if (millis() - ultimoControle < INTERVALO_CONTROLE){
    return;
  }

  ultimoControle = millis();

  lerSensores();

  bool esqVeLinha = leituraEsq >= LIMIAR_LINHA;
  bool dirVeLinha = leituraDir >= LIMIAR_LINHA;

  // cruzamento (os dois na linha): segue reto sem PD | se durar demais, saiu da pista: para

  if (leituraEsq >= LIMIAR_CRUZAMENTO && leituraDir >= LIMIAR_CRUZAMENTO){

    if (!noCruzamento){
      noCruzamento = true;
      inicioCruzamento = millis();
    }

    if (millis() - inicioCruzamento > TEMPO_MAX_CRUZAMENTO){
      desligarRobo();
      return;
    }

    instanteCruzamento = millis();
    ultimoLado = LADO_NENHUM;
    buscandoLinha = false;
    primeiroCalculo = true;

    controlarMotores(VELOCIDADE_BASE, VELOCIDADE_BASE);
    return;

  }

  noCruzamento = false;

  // guarda o lado quando a linha passa forte por um sensor (menos logo apos um cruzamento)

  if (millis() - instanteCruzamento >= TEMPO_CRUZAMENTO){

    int ladoForte = LADO_NENHUM;

    if (leituraEsq >= LIMIAR_LADO && leituraEsq > leituraDir){
      ladoForte = LADO_ESQUERDA;
    } else if (leituraDir >= LIMIAR_LADO && leituraDir > leituraEsq){
      ladoForte = LADO_DIREITA;
    }

    if (ladoForte != LADO_NENHUM){

      if (ultimoLado == -ladoForte && millis() - instanteUltimoLado <= JANELA_CRUZAMENTO){

        // a linha passou forte por um sensor e logo depois pelo outro: cruzamento torto, nao curva
        ultimoLado = LADO_NENHUM;
        instanteCruzamento = millis();

      } else {

        ultimoLado = ladoForte;
        instanteUltimoLado = millis();

      }
    }
  }

  bool perdeuLinha = !esqVeLinha && !dirVeLinha;

  // perdeu a linha pelo lado de fora da curva: comeca a busca

  if (!buscandoLinha && perdeuLinha &&
      ultimoLado != LADO_NENHUM &&
      millis() - instanteUltimoLado <= MEMORIA_LADO){

    buscandoLinha = true;
    ladoBusca = ultimoLado;
    inicioBusca = millis();
    ultimoLado = LADO_NENHUM;

  }

  if (buscandoLinha){

    if (perdeuLinha){

      if (millis() - inicioBusca > TEMPO_MAX_BUSCA){
        desligarRobo(); // nao achou: para
      } else {
        girarParaLado(ladoBusca);
      }

      return;

    }

    // achou a linha: volta para o PD
    buscandoLinha = false;
    primeiroCalculo = true;
    integralErro = 0;

    // achou logo em seguida com o sensor do OUTRO lado: era um cruzamento torto, nao curva

    bool achouDoOutroLado = (ladoBusca == LADO_DIREITA) ? esqVeLinha : dirVeLinha;

    if (achouDoOutroLado && millis() - inicioBusca <= JANELA_CRUZAMENTO){
      ultimoLado = LADO_NENHUM;
      instanteCruzamento = millis();
    }

  }

  controlePID();

}