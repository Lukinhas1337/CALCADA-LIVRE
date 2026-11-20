# Ligações (LDR, buzzer e 3 LEDs; nenhuma peça impressa)

**O que muda no carrinho:** componentes novos (LDR, resistor de 10 kΩ, buzzer ativo, 3 LEDs e 3 resistores de 220 Ω), todos na mini protoboard do kit. Tudo que já funciona (motores, driver, HC-SR04, pinos D12/D10/D2/D4/D11/D3/D6/D7) **continua igual**.

## Lista de material novo
| Item | Qtd | Observação |
|---|---|---|
| LDR de 5 mm (GL5528) | 1 (2 com reserva) | O sensor adicional |
| Resistor de 10 kΩ, 1/4 W | 1 | Par do LDR (marrom, preto, laranja) |
| Buzzer **ativo** de 5 V | 1 | Precisa ser **ativo**; o passivo não toca com este código |
| LED de 5 mm **verde**, **amarelo** e **vermelho** | 1 de cada (2 de cada com reserva) | — |
| Resistor de 220 Ω, 1/4 W | 3 (4 com reserva) | Um em série com cada LED (vermelho, vermelho, marrom) |
| Jumpers macho-macho e macho-fêmea | uns 8 | Os que já têm no kit |
| Mini protoboard | 1 | **Já vem no kit** |
| Papel preto, fita dupla face ou crepe | — | Viseira do LDR e fixação |

## Ligações novas

| Origem | Destino | Observação |
|---|---|---|
| Arduino **5V** | coluna "5V" | Mova também o fio de VCC do HC-SR04 para essa coluna: o soquete 5V só aceita um fio |
| Arduino **GND** (o que fica ao lado do AREF, na fileira de pinos digitais) | coluna "GND-A" | É o terceiro GND da placa; os outros dois já estão em uso |
| Coluna "GND-A" | coluna "GND-B" | Um jumperzinho curto: junta o GND de todos os LEDs |
| Um terminal do **LDR** | coluna "5V" | Jumpers macho-fêmea: a ponta fêmea encaixa na perna do LDR |
| Outro terminal do LDR | coluna "A0" | — |
| Arduino **A0** | coluna "A0" | — |
| **Resistor 10 kΩ** | entre a coluna "A0" e a coluna "GND-A" | — |
| Arduino **D5** | **perna (+)** do buzzer | A perna (+) é a mais longa ou vem marcada |
| Perna **(−)** do buzzer | coluna "GND-A" | — |
| Arduino **D13** → resistor 220 Ω → **LED verde** (perna longa) | cátodo (perna curta) na coluna "GND-B" | LED "caminho livre" |
| Arduino **D9** → resistor 220 Ω → **LED amarelo** | cátodo na coluna "GND-B" | LED "trecho escuro" |
| Arduino **D8** → resistor 220 Ω → **LED vermelho** | cátodo na coluna "GND-B" | LED "obstáculo ou risco". (o LED da placa não acompanha: o vermelho usa o D8) |

**Cada LED sempre leva seu resistor de 220 Ω em série** (sem ele o LED queima). A perna **longa** do LED é o ânodo (vai para o lado do Arduino); a **curta** é o cátodo (vai para o GND).

### Mapa das colunas da mini protoboard (sugestão)
| Coluna | O que fica nela |
|---|---|
| 1 "5V" | jumper do Arduino 5V · VCC do HC-SR04 · perna do LDR |
| 2 "A0" | outra perna do LDR · um lado do resistor de 10 kΩ · jumper para o A0 |
| 3 "GND-A" | outro lado do resistor de 10 kΩ · perna (−) do buzzer · jumper do Arduino GND · jumper para a coluna 5 |
| 4 "D5" | jumper do D5 · perna (+) do buzzer |
| 5 "GND-B" | cátodos (pernas curtas) dos 3 LEDs · jumper vindo da coluna 3 |
| 6, 7, 8 | um LED cada: jumper do D13 / D9 / D8 (verde / amarelo / vermelho) e uma ponta do resistor de 220 Ω na coluna do jumper; a outra ponta do resistor e o ânodo do LED numa coluna vizinha |

Em cada coluna de 5 furos (parte de cima ou de baixo do vão central) os furos estão ligados entre si. Deixe uma coluna vazia entre os grupos para não ligar nada por engano.

O divisor de tensão do LDR: 5V → LDR → **A0** → 10 kΩ → GND. **Mais luz = leitura maior.**

## Cuidados
- Não use `tone()` no buzzer: ele desliga o PWM dos pinos 3 e 11 (motores). O código usa `digitalWrite`.
- Antes de ligar, confira que **5V e GND não estão na mesma coluna** (curto) e que **cada LED tem seu resistor**.
- Se algum LED não acender, confira a polaridade (perna longa para o lado do Arduino).
- Se quiser usar só o LED da placa, grave o código com `LEDS_COMPLETOS 0` (não precisa dos 3 LEDs externos).
- Prenda os fios longe das rodas com fita ou abraçadeira.

## O que fica igual (não mexer)
D12/D10 → IN1/IN2 · D2/D4 → IN3/IN4 · D11/D3 → ENA/ENB · D6 → Trig · D7 → Echo · GND comum entre Arduino e driver · pilhas AA no L298N · bateria de 9 V no Arduino.
