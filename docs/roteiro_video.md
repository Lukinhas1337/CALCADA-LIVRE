# Roteiro de narração: vídeo "Calçada Livre" (5:08)

Ordem do seu vídeo: abertura (8 s), carrinho parado com o Monitor Serial (1:39), falha do LDR (1:18) e 4 takes do carrinho andando (2:03). Os tempos abaixo são do vídeo **inteiro**, já com os blocos juntos. O texto cabe no tempo de cada trecho (cerca de 2,3 palavras por segundo). Assista uma vez com o roteiro ao lado e **troque as frases que não bateram com o que você vê**.

## 0:00 – 0:08 · Abertura (8 s)
"Olá! Somos o grupo do projeto Calçada Livre, do Project-Based Maker Lab da Global Solution 2026.2."

## 0:08 – 1:47 · Carrinho parado com o Monitor Serial (1:39)

| Tempo | O que aparece | Narração |
|---|---|---|
| **0:08 – 0:26** | Monitor Serial, depois o dedo apertando o reset no Arduino | "Calçadas com obstáculos e pouca luz são um risco para idosos e pessoas com deficiência. O Censo 2022 do IBGE contou 14,4 milhões de pessoas com deficiência no Brasil. O nosso carrinho inspeciona a calçada sozinho e avisa quem está por perto." |
| **0:26 – 0:38** | O carrinho parado em cima do computador | "Ele usa o sensor ultrassônico, que já tínhamos na Etapa 1, e um sensor novo, o LDR, que mede a luz. A decisão é toda do Arduino, e o aviso sai por três LEDs e um buzzer." |
| **0:38 – 0:44** | Serial com a linha C e as leituras LIVRE | "Ao ligar, o Arduino calibra por dois segundos e meio: mede a luz do ambiente e define a referência. A linha C mostra essa referência e os limites de escuro." |
| **0:44 – 0:52** | Mão perto do sensor, LED vermelho, Serial com OBSTACULO e MANOBRA | "Com a mão a menos de 20 centímetros do ultrassônico, a situação vira OBSTÁCULO. O LED vermelho acende, o buzzer dá dois bips e o carro entra em manobra." |
| **0:52 – 1:08** | Sala escura: LED verde, depois amarelo, depois vermelho | "No claro o LED é verde. Quando a luz cai abaixo do limite, a situação vira ESCURO: LED amarelo e um bip. Com um obstáculo no escuro é risco alto: LED vermelho piscando e buzzer contínuo." |
| **1:08 – 1:34** | Monitor Serial com as linhas D (ESCURO, RISCO_ALTO, LIVRE) | "No Monitor Serial, cada linha D traz o tempo, a distância, a luz, a situação e o modo. Dá para ver a luz caindo e a situação virar ESCURO. No risco alto o carro fica em ESPERA_RISCO, parado até a situação normalizar. Depois ele volta para LIVRE e continua a patrulha." |
| **1:34 – 1:47** | Closes no carro: ultrassônico, protoboard, LEDs e buzzer | "Aqui está o hardware: Arduino Uno, driver L298N e quatro motores. Na frente fica o ultrassônico. Na mini protoboard ficam o LDR, com um resistor de 10 quilo-ohms, os três LEDs e o buzzer." |

## 1:47 – 3:05 · Falha do LDR (1:18)

| Tempo | O que aparece | Narração |
|---|---|---|
| **1:47 – 2:05** | Serial, close no carro, depois o carro com o LED verde | "Agora o teste de falha. O carro está funcionando normalmente, com a situação LIVRE e o LED verde." |
| **2:05 – 2:27** | Mão cobrindo o LDR, LED amarelo, Serial com ESCURO | "Quando cobrimos o LDR, a luz cai e a situação vira ESCURO, com o LED amarelo. É a leitura normal do sensor." |
| **2:27 – 2:31** | Serial com FALHA e FALHA, SEGURO | "Quando o LDR fica sem leitura por cerca de 3 segundos, o programa registra FALHA e entra em modo SEGURO. O carro para, os LEDs piscam e o buzzer dá três bips." |
| **2:31 – 2:49** | **Tela preta (18 s).** Só o som do alarme | **Recomendo cortar esse trecho.** Se cortar, subtraia 18 s dos tempos daqui para frente. Se mantiver, leia só: "Enquanto a falha continua, o carro não anda às cegas." |
| **2:49 – 2:57** | Serial com FALHA_OK e depois ESCURO, LIVRE | "Ao reconectar o sensor, aparece FALHA_OK e o carro volta ao normal, sem precisar reiniciar." |
| **2:57 – 3:05** | Relatório no Serial | "No fim da rota, o relatório: 1 obstáculo, 3 trechos escuros, nenhum risco alto e 6 falhas em 60 segundos. As falhas vêm principalmente de cobrir o LDR por completo, porque leitura zero por mais de 3 segundos conta como falha do sensor." |

## 3:05 – 5:08 · Carrinho andando (2:03)

Os tempos assumem os takes na ordem 1 a 4 (piso de cerâmica, depois piso de madeira). Se a ordem for outra, troque os textos de lugar.

| Tempo | O que aparece | Narração |
|---|---|---|
| **3:05 – 3:48** | Take 1, no piso de cerâmica, com a caixa de papelão | "Agora o teste de verdade, no chão, sem cabo, só com pilhas e bateria. O carro anda sozinho, se aproxima da caixa e reage ao obstáculo. Os LEDs e o buzzer avisam o que está acontecendo, para que a pessoa perceba mesmo sem olhar para o carro." |
| **3:48 – 4:03** | Take 2, no piso de madeira, perto da mesa | "Segundo teste, em outro piso. O carro passa para uma área com mais sombra, e dá para ver o LED e ouvir o buzzer reagirem." |
| **4:03 – 4:28** | Take 3, o carro e a caixa | "Terceiro teste: o carro se aproxima da caixa e desvia sozinho. O ultrassônico detecta o obstáculo e o carro muda de direção." |
| **4:28 – 4:48** | Take 4, o carro se afasta da caixa | "Quarto teste. Os resultados dos testes estão registrados na tabela do repositório, junto com fotos e o código." |
| **4:48 – 5:08** | Final (última imagem ou cartão) | "Com um sensor adicional, o LDR, o carro decide sozinho em quatro situações: obstáculo, trecho escuro, risco alto e falha do sensor. Como limitações, ele não usa GPS nem câmera e depende da luz ambiente. Como próximos passos, sensores de desnível e envio dos dados da rota. Obrigado!" |

## Antes de gravar a voz
- **Os 20 s a mais dos takes (123 s no total, enquanto os 4 arquivos somam cerca de 103 s)** podem estar em pausas ou em um take mais longo. Ajuste os tempos dos takes pelo que você vê.
- Nos takes, **descreva o que realmente acontece**. Não vi todos os desvios completos, então confirme o lado para onde o carro desvia e se parou antes da caixa.
- A tabela de testes do repositório já está preenchida com o que aparece nos vídeos (1 tentativa por cenário). Não diga que foram dezenas de testes.
- Deixe 1 s de silêncio no começo e no fim de cada bloco, para cortar e alinhar mais fácil.
- Deixe o áudio original do vídeo em cerca de 25 a 35% e a sua voz em 100%. O som do buzzer faz parte da demonstração.

## Depois do vídeo
Suba no YouTube como **não listado**, abra o link numa janela anônima para testar e cole no README, no lugar do PREENCHER. O prazo é hoje às 23:55.
