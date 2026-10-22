# Buzzline

Robô seguidor de linha com a placa **RoboCore BlackBoard Rocky** (ESP32), programado em C++ com o PlatformIO no VS Code.

O robô usa dois sensores de refletância e controle proporcional para seguir uma linha preta sobre fundo branco.

---

## Como configurar em um computador novo

### 1. Ativar caminhos longos (somente Windows)

A plataforma do ESP32 tem arquivos com caminhos muito longos, e o Windows bloqueia caminhos acima de 260 caracteres por padrão. Sem este passo, a instalação falha com `FileNotFoundError`.

1. Abra o **PowerShell como administrador**: menu Iniciar, digite "PowerShell", clique com o botão direito e escolha **Executar como administrador**.
2. Rode o comando:

   ```powershell
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
   ```

3. **Reinicie o computador.**

> No Mac ou no Linux, pule este passo.

**Sem acesso de administrador?** Use um caminho curto para o PlatformIO:

1. No menu Iniciar, procure "variáveis de ambiente" e abra **Editar as variáveis de ambiente para sua conta**.
2. Crie uma variável do usuário chamada `PLATFORMIO_CORE_DIR` com o valor `C:\pio`.
3. Faça isso **antes** de instalar a extensão do PlatformIO.

### 2. Instalar os programas

- [Git](https://git-scm.com/downloads)
- [VS Code](https://code.visualstudio.com/)
- Extensão **PlatformIO IDE** no VS Code: abra a aba de Extensões, procure "PlatformIO IDE" e instale. Na primeira vez, a extensão leva alguns minutos para instalar o próprio núcleo.

### 3. Baixar o projeto

Na primeira vez, clone o repositório:

```bash
git clone https://github.com/<seu-usuario>/Buzzline.git
```

Depois, no VS Code, use **File → Open Folder** e abra a pasta `Buzzline`.

Para pegar atualizações depois, use:

```bash
git pull
```

### 4. Compilar

Clique no **✓ (Build)** na barra azul de baixo do VS Code.

No primeiro Build, o PlatformIO baixa sozinho a plataforma do ESP32, o compilador e a biblioteca `RoboCore_Rocky`. São algumas centenas de MB, então pode demorar. O Build terminou bem quando aparece **SUCCESS** no terminal.

Se o `#include "buzzline.h"` continuar sublinhado em vermelho, abra a paleta com `Ctrl+Shift+P` e rode **PlatformIO: Rebuild IntelliSense Index**.

### 5. Gravar na placa

A Rocky não tem USB próprio: a gravação é feita por um **conversor USB-UART**.

1. **Desconecte a bateria.** A placa não tem seletor de alimentação, então nunca use bateria e conversor ao mesmo tempo.
2. Encaixe o conversor nos pinos de programação da Rocky (GND com GND) e ligue o USB no computador.
3. Confira se a barra azul mostra `env:rocky`.
4. Clique na **→ (Upload)**.
5. Se travar em `Connecting...`, segure o botão **BOOT** da placa até a gravação começar.

Se o Upload não encontrar a porta, instale o driver do conversor USB-UART ou troque o cabo USB.

---

## Ligações

| Componente | Pino da Rocky |
| --- | --- |
| Sensor esquerdo | VP (GPIO36) |
| Sensor direito | VN (GPIO39) |
| Motor esquerdo | Motor A |
| Motor direito | Motor B |
| Botão start/stop | BOOT da placa (GPIO0), não precisa ligar nada |
| LED de status | LED "L" da placa (GPIO15) |

- O sinal dos sensores deve ser de **no máximo 3,3 V**.
- A bateria dos motores deve ter de **7 a 11 V**.

---

## Se as ligações do robô forem diferentes

Se o robô foi montado com outros pinos, dá para ajustar tudo pelo código, sem mexer no hardware. Primeiro descubra onde cada coisa está ligada: pergunte à equipe que montou o robô ou siga os fios até a placa.

### Pinos que a Rocky já usa

Um botão, LED ou sensor novo **não** pode estar em um destes pinos:

| Pino | Usado por |
| --- | --- |
| 1 e 3 | Gravação (conversor USB-UART) |
| 0 | Botão BOOT |
| 4, 13, 14, 25, 26, 27 | Motores |
| 15 | LED da placa |
| 33 | Liga/desliga os drivers dos motores |
| 34 | Leitura da bateria |
| 36 e 39 | Sensores (VP e VN) |

### Sensores

Troque os números em `include/buzzline.h`:

```cpp
const int PINO_SENSOR_ESQ = 36;   // troque pelo pino real
const int PINO_SENSOR_DIR = 39;
```

- O pino precisa ter entrada analógica: **32, 33, 34, 35, 36 (VP) ou 39 (VN)**.
- Os pinos 5, 16, 17, 18, 19, 21, 22 e 23 **não** leem valores analógicos. Se os sensores estiverem num deles, provavelmente são sensores digitais (só preto/branco), e a leitura precisa ser adaptada.
- Se o robô fugir da linha em vez de segui-la, os sensores estão trocados: troque os dois números entre si.

### Motores

Os motores não têm número de pino no código; quem define é a saída da Rocky (A, B ou C) onde cada roda está ligada. Os ajustes ficam na função `controlarMotores()`, em `src/buzzline.cpp`:

- **Rodas nas saídas A e B, mas trocadas de lado:** troque `setSpeedLeft` e `setSpeedRight` entre si.
- **Alguma roda na saída C:** use `setSpeedA`, `setSpeedB` ou `setSpeedC` no lugar de `setSpeedLeft`/`setSpeedRight`, conforme a saída de cada roda.
- **Roda girando ao contrário:** mude `INVERTER_MOTOR_ESQ` ou `INVERTER_MOTOR_DIR` para `true` em `include/buzzline.h`.

### Botão externo

**Botão ligado entre o pino e o GND** (o mais comum): só troque o número em `include/buzzline.h`.

```cpp
const int PINO_BOTAO = 0;   // troque pelo pino do botão
```

**Botão ligado entre o pino e o 3,3 V:** além de trocar o número, edite `src/buzzline.cpp`:

1. No `iniciarPinos()`, troque `INPUT_PULLUP` por `INPUT_PULLDOWN`.
2. No `verificarBotao()`, troque as duas comparações `== LOW` por `== HIGH`.

> Os pinos 34 a 39 não têm resistor interno. Com o botão ligado ao 3,3 V num desses pinos, o circuito precisa ter um resistor de pull-down externo.

### LED externo

1. Em `include/buzzline.h`, adicione o pino do LED:

   ```cpp
   const int PINO_LED = 2;   // troque pelo pino do LED
   ```

2. Em `src/buzzline.cpp`, troque a linha que cria o LED:

   ```cpp
   RockyLED led(PINO_LED);   // antes era: RockyLED led;
   ```

Isso vale para o LED ligado do pino para o GND, com resistor. Se ele estiver ligado do 3,3 V para o pino, vai funcionar invertido: troque `led.on()` e `led.off()` entre si no `verificarBotao()`.

---

## Como usar

1. Ligue o robô em cima da pista. O LED pisca por 4 segundos: é a **calibração**.
2. Durante esse tempo, passe os sensores sobre a linha preta e sobre o branco algumas vezes.
3. Centralize o robô na linha e aperte o **BOOT**. O LED acende e o robô começa a andar.
4. Aperte o **BOOT** de novo para parar.

Com a placa ligada no conversor, o Serial Monitor (115200 baud) mostra os valores medidos na calibração.

---

## Ajustes

Os ajustes ficam em `include/buzzline.h`.

| Constante | Valor atual | O que faz |
| --- | --- | --- |
| `VELOCIDADE_BASE` | 60 | Velocidade em linha reta (0 a 100 %) |
| `KP` | 0.6 | Força da correção. Diminua se o robô ziguezaguear e aumente se ele sair nas curvas |
| `TEMPO_CALIBRACAO` | 4000 | Duração da calibração, em ms |
| `INVERTER_MOTOR_ESQ` / `INVERTER_MOTOR_DIR` | false | Mude para `true` se uma roda girar ao contrário |

---

## Estrutura do projeto

```
Buzzline/
├── include/
│   └── buzzline.h      # pinos, ajustes e declarações
├── src/
│   ├── buzzline.cpp    # sensores, motores, controle e botão
│   └── main.cpp        # setup() e loop()
└── platformio.ini      # placa, plataforma e bibliotecas
```

Biblioteca usada: [RoboCore_Rocky](https://github.com/RoboCore/RoboCore_Rocky)