# Limiares e filtros: valor, motivo e como ajustar

| Parâmetro | Valor | Por quê | Como ajustar |
|---|---|---|---|
| `dist_entrar_cm` | 20 cm | Espaço para o robô parar e fazer a ré em baixa velocidade | Aumente se ele bater antes de parar |
| `dist_sair_cm` | 25 cm | **Histerese de 5 cm**: sem ela, uma leitura oscilando entre 19 e 21 cm faria o robô alternar entre "obstáculo" e "livre" | Mantenha pelo menos 3 a 5 cm acima do valor de entrada |
| Janela do ultrassônico | Mediana de 5 leituras (a cada 60 ms) | O HC-SR04 dá picos isolados (eco falso). A mediana ignora 1 ou 2 leituras ruins e detecta um obstáculo real em até 3 leituras (180 ms) | Janela maior filtra mais, mas reage mais devagar |
| Intervalo de leitura | 60 ms | O HC-SR04 precisa de uns 50 ms entre disparos para não captar o próprio eco | Não baixe de 50 ms |
| `frac_escuro_entrar` | 50% da referência | Limites **relativos** à luz medida na calibração funcionam em salas com iluminações diferentes | Suba (ex.: 60%) para o robô detectar escuro mais cedo |
| `frac_escuro_sair` | 65% da referência | **Histerese** de 15 pontos percentuais: a luz tremendo perto do limite não faz o LED piscar | Mantenha acima do valor de entrada |
| Janela do LDR | Média de 8 leituras | O LDR varia com sombra de mão e flicker de lâmpada | — |
| `luz_ref_minima` | 150 | Referência abaixo disso indica calibração feita no escuro; o código avisa (3 bips + mensagem) e usa 500 | Ajuste se o LDR/resistor forem diferentes |
| `ldr_falha_ms` | 3 s com leitura ≤ 1 ou ≥ 1022 | LDR desconectado lê ~0. Escuro real (mão cobrindo) fica acima de 1. Os 3 s evitam falsa falha por sombra rápida | — |
| `echo_preso_leituras` | 3 | Com `INPUT_PULLUP`, fio de Echo solto lê ALTO antes do disparo | — |
| `sem_eco_falha_ms` | 0 (desligado) | Em ambiente aberto não ter eco é normal; ligado, o robô entraria em modo seguro sozinho numa sala grande | Em sala fechada, com paredes por perto, pode usar 6000 (6 s sem eco = sensor sem resposta) |
| `recuperacao_ms` | 1 s | Leituras normais por 1 s antes de sair do modo seguro, para não alternar | — |
| `VEL_LENTA` | 190 | Reduz a velocidade no escuro sem travar os motores (pilhas AA entregam pouca tensão) | Aumente se travar; diminua para ir mais devagar |
| `RISCO_TIMEOUT_MS` | 8 s | Se o obstáculo não sair, o robô tenta desviar em vez de ficar parado para sempre | — |

> Todos os números do código são **valores de partida**. Anotem em `docs/calibracao.md` os valores que vocês realmente mediram.
