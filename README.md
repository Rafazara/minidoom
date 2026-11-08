```text
┌────────────────────────────────────────────┐
│ ███╗   ███╗██╗███╗   ██╗██╗                │
│ ████╗ ████║██║████╗  ██║██║                │
│ ██╔████╔██║██║██╔██╗ ██║██║                │
│ ██║╚██╔╝██║██║██║╚██╗██║██║                │
│ ██║ ╚═╝ ██║██║██║ ╚████║██║                │
│ ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝                │
│               D O O M                      │
│      BIG DEMONS. TINY PIXELS.              │
└────────────────────────────────────────────┘
```

# MINI-DOOM

**Um FPS inspirado em Doom em uma telinha monocromática de 128×64 pixels.**

MINI-DOOM explora raycasting, sprites, framebuffer 1-bit e arquitetura de jogos em um **ESP32 DOIT DevKit V1** com **OLED SSD1306 I²C**. É um projeto de engenharia embarcada e retrocomputação — não um port do engine original do Doom.

## Status do projeto

| Frente | Estado atual |
|---|---|
| Compilação ESP32 | **PASS** — firmware principal e diagnóstico compilam |
| Diagnóstico OLED no hardware | **PASS** — funcionamento confirmado pelo usuário no hardware corrigido |
| Gameplay no hardware | **PENDENTE** — controles, combate, waves e reset ainda não têm validação completa |
| Testes automatizados locais | **BLOQUEADO** — o host Windows não possui GCC/G++; a CI executa a suíte nativa |
| Release | **PARTIAL** — baseline recuperada; não é uma versão final validada |

O diagnóstico valida o caminho de exibição, não todo o jogo. O alvo de 20 FPS é uma configuração, **não uma medição**.

## Por que este projeto existe?

Executar um jogo em primeira pessoa em uma tela de 128×64 pixels exige decisões diferentes das de um PC. Cada byte do framebuffer importa, cada transferência I²C consome tempo e cada pixel precisa chegar à posição certa.

MINI-DOOM transforma essas restrições em um pequeno laboratório jogável:

- raycasting para criar paredes com perspectiva;
- dithering para sugerir tonalidades em apenas duas cores;
- sprites com oclusão por profundidade;
- HUD, combate, waves e telas de estado;
- diagnósticos separados para testar o display sem executar o jogo.

O ponto de partida utiliza dados e conceitos do [Doom Nano](https://github.com/daveruiz/doom-nano), com adaptação própria para ESP32 e Adafruit SSD1306.

## Hardware

| Componente | Configuração confirmada |
|---|---|
| Placa | ESP32 DOIT DevKit V1; ESP32-D0WDQ6 observado em uploads anteriores |
| Flash | 4 MB reportados pelo PlatformIO |
| Display | OLED 128×64 I²C, compatível com SSD1306 |
| Endereço I²C | `0x3C`, confirmado por scan físico |
| SDA / SCL | GPIO21 / GPIO22 |
| Alimentação do OLED | 3.3 V e GND comum |
| Monitor serial | 115200 baud; COM7 era a porta local utilizada |

| Pino do OLED | Conexão no ESP32 |
|---|---|
| GND | GND |
| VCC | 3.3V |
| SCL / SCK | GPIO22 |
| SDA | GPIO21 |

```text
ESP32 DOIT DevKit V1                 OLED 128×64
+-------------------+                +-----------+
| 3.3V              |--------------->| VCC       |
| GND               |--------------->| GND       |
| GPIO22 (SCL)      |--------------->| SCL/SCK   |
| GPIO21 (SDA)      |--------------->| SDA       |
| USB               |<-- dados e alimentação --- PC
+-------------------+                +-----------+
```

Somente ESP32 e OLED formam o baseline confirmado. Botões, buzzer, bateria e case não são presumidos. Desligue a alimentação antes de alterar a fiação; o ESP32 usa lógica de 3.3 V.

Detalhes e segurança: [docs/HARDWARE.md](docs/HARDWARE.md).

## Como funciona

```text
Entrada opcional
      |
      v
DoomAdapter — estados, movimento, combate e reset
      |
      +--> Raycasting --> framebuffer do mundo + depth buffer
      |
      +--> Sprites ----+
      +--> HUD --------+--> framebuffer final de 1.024 bytes
      +--> Efeitos ----+             |
                                     v
                         Cópia exata para o buffer Adafruit
                                     |
                                     v
                              Wire / I²C / OLED
```

O framebuffer usa o formato nativo de páginas do SSD1306: cada byte contém oito pixels verticais. Não há conversão por `drawBitmap()` no caminho principal.

A arquitetura completa está em [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Funcionalidades e evidências

| Funcionalidade | Implementação | Build | Hardware |
|---|---|---|---|
| Framebuffer 128×64 e diagnóstico OLED | Implementada | PASS | **PASS no diagnóstico** |
| Raycasting, texturas e dithering | Implementada | PASS | Pendente no gameplay |
| Sprites, arma e depth buffer | Implementada | PASS | Pendente |
| TITLE / PLAYING / GAME_OVER | Implementada | PASS | Pendente |
| Movimento, colisão e combate | Implementada | PASS | Pendente |
| Vida, munição, waves e reset | Implementada | PASS | Pendente |
| Entrada por cinco botões opcionais | Implementada | PASS | Não confirmada conectada |

## Quick start

### Pré-requisitos

- Git com suporte a submodules.
- Python suportado e PlatformIO Core (`pio`).
- ESP32 e OLED conectados conforme a tabela.
- Cabo USB de dados.

### Clonar e compilar

```powershell
git clone --recurse-submodules https://github.com/Rafazara/minidoom.git
cd minidoom
git submodule update --init --recursive
pio run -e esp32doit-devkit-v1
```

As versões de Espressif32, Adafruit GFX e Adafruit SSD1306 estão fixadas em `platformio.ini`.

### Compilar o diagnóstico

```powershell
pio run -e esp32doit-devkit-v1-oled-diag
```

### Upload e monitor

Revise `upload_port` e `monitor_port` antes do upload. COM7 é apenas a configuração local conhecida.

```powershell
pio run -e esp32doit-devkit-v1 -t upload
pio device monitor -p COM7 -b 115200
```

Consulte [docs/BUILD_AND_FLASH.md](docs/BUILD_AND_FLASH.md) para builds limpos e solução de problemas.

## Diagnóstico OLED

O ambiente isolado apresenta telas apagada/cheia, pixels de referência, linhas, grid, checkerboard, borda, marcadores de canto, texto e um quadrado móvel, sem inicializar AI ou combate.

**Resultado físico atual:** o usuário confirmou que o firmware OLED corrigido funcionou no hardware. Isso estabelece o diagnóstico como validado em escopo geral; não substitui o checklist por padrão nem valida o gameplay.

Para reproduzir:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag -t upload
pio device monitor -p COM7 -b 115200
```

Para retornar ao jogo:

```powershell
pio run -e esp32doit-devkit-v1 -t upload
```

Checklist e interpretação: [docs/OLED_DIAGNOSTICS.md](docs/OLED_DIAGNOSTICS.md).

## Controles opcionais

O firmware mapeia botões active-low com pull-ups internos:

| Ação | GPIO |
|---|---|
| Frente | 32 |
| Trás | 33 |
| Strafe esquerdo | 25 |
| Strafe direito | 26 |
| Fire / iniciar / reiniciar | 27 |

Nenhum botão está confirmado no baseline atual. Sem Fire, o firmware normal permanece em TITLE.

## Memória e desempenho

| Medição do build principal | Resultado |
|---|---:|
| RAM estática reportada | 24.516 / 327.680 bytes (**7,5%**) |
| Flash | 329.185 / 1.310.720 bytes (**25,1%**) |
| Framebuffers do projeto | 2 × 1.024 bytes, estáticos |
| Buffer Adafruit adicional | 1.024 bytes alocados em runtime |
| Alvo configurado | 20 FPS / 50 ms |
| Clock I²C atual | 100 kHz |

A RAM reportada pelo PlatformIO não inclui todas as alocações de runtime. A 100 kHz, transferir um frame inteiro leva aproximadamente 90 ms ou mais com overhead; portanto 20 FPS não é alcançável nessa configuração. Qualquer aumento do clock exige validação elétrica e no display.

## Testes e CI

```powershell
pio test -e native
```

As suítes cobrem offsets, bit packing, limites do framebuffer e decodificação do mapa. No host Windows de recuperação, GCC/G++ não estão disponíveis. O workflow de GitHub Actions compila os dois firmwares e executa esses testes em Ubuntu; o resultado verificável aparece nos checks do PR.

Veja [docs/TESTING.md](docs/TESTING.md) e [docs/RELEASE_STATUS.md](docs/RELEASE_STATUS.md).

## Galeria

Ainda não há fotografias próprias do hardware MINI-DOOM no repositório. As imagens dentro de `third_party/doom-nano` pertencem ao projeto upstream e não são apresentadas como fotos desta montagem.

Para completar a galeria com evidências reais:

- foto frontal do ESP32 + OLED na breadboard;
- diagnóstico com borda e cantos visíveis;
- TITLE legível;
- gameplay real com HUD;
- detalhe da fiação SDA/SCL e alimentação;
- case somente se um for efetivamente construído.

## Troubleshooting

| Problema | Ação inicial |
|---|---|
| `avr/pgmspace.h` não encontrado | Inicialize o submodule e use a árvore atual; o wrapper está em `include/avr/pgmspace.h` |
| Display configurado como 128×32 | Use `OLED_HEIGHT=64`; assertions bloqueiam divergência do framebuffer |
| OLED não detectado | Verifique 3.3 V, GND, SDA=21, SCL=22 e endereço `0x3C` |
| Porta de upload indisponível | Confira a porta real, feche monitors e revise `platformio.ini` |
| OLED apagado ou corrompido | Execute o diagnóstico isolado antes de alterar gameplay ou orientação |
| Dependências PlatformIO variando | Use as versões fixadas e preserve o submodule; não atualize tudo indiscriminadamente |
| Testes nativos não compilam no Windows | Disponibilize GCC/G++ ou use o job Ubuntu da CI |

## Histórico de desenvolvimento

- Integração inicial de ESP32, I²C, OLED e Doom Nano.
- Construção incremental de raycasting, sprites, HUD, estados, combate e waves.
- Investigação de corrupção do OLED e revisão do formato do framebuffer.
- Recuperação da compatibilidade AVR→ESP32 e correção do overflow de 512 bytes.
- Diagnóstico OLED corrigido confirmado no hardware pelo usuário.
- Consolidação da documentação, testes nativos e automação de builds.

Os relatórios históricos na raiz foram preservados; os guias em `docs/` são a referência atual.

## Roadmap

1. Validar TITLE, movimento, combate, GAME_OVER e reset no hardware.
2. Arquivar o checklist visual por padrão e fotos reais do dispositivo.
3. Medir FPS, tempo de render/I²C, heap livre e estabilidade prolongada.
4. Controlar o log serial no caminho de release.
5. Revisar permissões dos sprites upstream e demais obrigações de terceiros.
6. Preparar uma release candidate somente após os critérios de aceite.

## Documentação

- [Arquitetura](docs/ARCHITECTURE.md)
- [Hardware e fiação](docs/HARDWARE.md)
- [Build e flash](docs/BUILD_AND_FLASH.md)
- [Diagnóstico OLED](docs/OLED_DIAGNOSTICS.md)
- [Portabilidade AVR/ESP32](docs/PORTABILITY.md)
- [Testes](docs/TESTING.md)
- [Status de release](docs/RELEASE_STATUS.md)

## Créditos e licença

O código original de MINI-DOOM é distribuído sob a [MIT License](LICENSE), já existente na branch padrão do repositório.

- [Doom Nano](https://github.com/daveruiz/doom-nano) — David Ruiz e contribuidores; submodule fixado pelo superprojeto.
- Doom Nano credita o material de raycasting de [lodev.org](https://lodev.org/cgtutor) e identifica fontes externas de sprites em seu README.
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library), [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) e Adafruit BusIO.
- Arduino ESP32 e PlatformIO.

A licença MIT do projeto não substitui notices nem direitos de terceiros. O snapshot upstream não possui uma licença repository-wide, e as permissões dos sprites externos ainda precisam de revisão. Doom e seus assets pertencem aos respectivos titulares; nenhuma afiliação ou endosso é reivindicado.
