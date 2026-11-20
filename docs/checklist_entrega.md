# Checklist de entrega (prazo: 19/11 às 23h55)

## Regras da disciplina
- [ ] Só vale **link do GitHub** (ZIP, RAR, DOC, DOCX e PDF **não** são aceitos)
- [ ] Repositório **público** (teste em janela anônima)
- [ ] **Nomes e RM de todos** no README (sem isso o grupo fica **sem nota na GS**)
- [ ] Vídeo de até **6 min** com o link no README
- [ ] **Só um** integrante entrega, no Teams da disciplina

## Conteúdo do repositório
- [ ] Código-fonte (`firmware/`) e instruções de instalação e execução (README)
- [ ] Descrição dos componentes e da integração entre os sensores (README)
- [ ] 4 fotos do protótipo em `docs/fotos/`
- [ ] `docs/calibracao.md` com os valores medidos
- [ ] `docs/testes_campo.csv` preenchido e a tabela colada no README
- [ ] Todos os `PREENCHER` resolvidos: `grep -rn PREENCHER .`

## Cronograma (prazo curto: quarta 18/11 e quinta 19/11)
| Quando | O que fazer |
|---|---|
| **Qua 18/11, tarde** | Conferir as peças; testar os motores com as rodas levantadas (`firmware/teste_motores`); montar o circuito na protoboard **sem ligar a bateria**; conferir as ligações |
| **Qua 18/11, noite** | Gravar `calcada_livre.ino` com `MOTORES_ATIVOS 0`; calibrar; testar LDR, obstáculo, LEDs e buzzer; depois `MOTORES_ATIVOS 1` no cenário de teste; fotos; testes rápidos |
| **Qui 19/11, manhã** | Gravar o vídeo; preencher os `PREENCHER` do README; publicar no GitHub |
| **Qui 19/11, até o início da tarde** | Testar o repositório em janela anônima e **entregar no Teams**. Não deixe para as 23h55 |

Plano B se o carrinho não andar direito: demonstre o sensor novo com `MOTORES_ATIVOS 0` (o robô parado reage com LEDs e buzzer) e explique a limitação no vídeo e no README.
