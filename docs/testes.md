# Plano de testes (versão mínima)

Registre cada tentativa em `testes_campo.csv` e gere a tabela com `python tools/resumo_testes.py`.
Iluminações: `claro` e `escuro` (ligue/calibre o robô **na luz de cada condição**).

| Cenário (`cenario`) | Como provocar | Resultado esperado | Tentativas |
|---|---|---|---|
| `trecho_escuro` | Entrar na zona escura (ou cobrir o LDR) | Anda devagar, LED amarelo, 1 bip | 5 por iluminação |
| `obstaculo_a_15cm` | Caixa a 15 cm, com luz normal | Para, 2 bips, LED vermelho, ré, gira e segue | 5 por iluminação |
| `obstaculo_no_escuro` | Caixa no trecho escuro (embaixo da mesa, túnel ou LDR coberto) | Para, alarme contínuo, LED vermelho piscando; volta quando a caixa sai | 5 por iluminação |
| `falha_ldr` | Desconectar o fio do A0 por mais de 3 s | Modo seguro: para, os 3 LEDs piscando, 3 bips repetidos; reconectar e recupera | 2 |
| `falha_echo` | Desconectar o fio do pino 7 | Modo seguro em menos de 1 s | 2 |

O plano completo teria cerca de 34 tentativas; **nesta entrega registramos 1 tentativa por cenário, observada nos vídeos**. Para cada uma, anote em `testes_campo.csv` se reagiu como esperado (`ok` ou `falha`) e, se der, o tempo de reação em ms.

## Tabela de resultados (gerada pelo script; **só preencham com o que foi medido**)

| Cenario | Iluminacao | Tentativas | Acertos | Taxa | Tempo medio de reacao |
|---|---|---|---|---|---|
| falha_ldr | claro | 1 | 1 | 100% | - |
| obstaculo_a_15cm | claro | 1 | 1 | 100% | - |
| obstaculo_no_escuro | escuro | 1 | 1 | 100% | - |
| trecho_escuro | claro | 1 | 1 | 100% | - |
| trecho_escuro | escuro | 1 | 1 | 100% | - |

## Testes automatizados no PC (já rodam)
`bash tests/run_tests.sh`: **94 verificações** (lógica de decisão, firmware com 3 LEDs e firmware com só o LED da placa). Eles provam que o **código** decide certo; **não substituem** os testes no carrinho, porque sensores reais têm ruído e o piso e as pilhas mudam o comportamento.

## Testes no chão (observação em vídeo, sem contagem de acertos)
Quatro gravações do carro andando sem cabo, só com pilhas (`docs/fotos/07` e `08`): piso de cerâmica e de madeira, com uma caixa de papelão como obstáculo, e uma passagem por área sombreada. O carro anda sozinho, aproxima-se da caixa e segue pelo percurso. Não medimos tempo de reação nem registramos acerto por tentativa nesses vídeos, por isso eles não entram na tabela acima.
