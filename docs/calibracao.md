# Calibração

## 1. Sensor de luz (LDR)
1. Ligue 5V → LDR → **A0** → resistor de 10 kΩ → GND.
2. Compile com `MOTORES_ATIVOS 0` e `LOG_DETALHADO 1` e abra o Monitor Serial em 9600.
3. Anote a leitura `luz` em **ambiente claro** e **com a mão cobrindo** o sensor. Preencha:

| Condição | Leitura média |
|---|---|
| Ambiente claro | 212 e 355 (referências calibradas em duas sessões, linhas `C,212,106,137` e `C,355,177,230`) |
| Sala mais escura | cerca de 44 a 66 (vídeo `IMG_6560`) |
| Mão cobrindo (parcial) | cerca de 84 a 118; cobrindo por completo a leitura vai a 0 e, após 3 s, vira FALHA |

4. O robô **não usa um valor fixo**: ao ligar, ele mede a luz por 2,5 s e calcula o limite (50% e 65% dessa referência). Por isso **ligue-o sempre com o sensor apontado para a luz normal da rota**. A linha `C,ref,entrar,sair` mostra o que ele calculou.
5. Se aparecer "AVISO: luz de referência muito baixa", reinicie a placa em ambiente claro.

## 2. Ultrassônico (HC-SR04)
Não fizemos a medição com régua nesta entrega. O limite de obstáculo (20 cm, saída em 25 cm) foi verificado no teste de bancada com a mão (`E,...,OBSTACULO`) e nos testes no chão. Para medir: coloque um obstáculo a 10, 20, 30 e 50 cm e compare com `dist` do Serial.

| Distância real | Leitura (3 medidas) | Erro |
|---|---|---|
| 10 cm | não medido com régua | n/a |
| 20 cm | não medido com régua | n/a |
| 30 cm | não medido com régua | n/a |
| 50 cm | não medido com régua | n/a |

## 3. Velocidades
Com o carrinho no chão, teste `VEL_NORMAL` e `VEL_LENTA` e anote os valores que funcionam com as pilhas que serão usadas na demonstração. Use pilhas novas.
