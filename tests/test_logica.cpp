// Testes da logica de decisao. Compilar e rodar:  bash tests/run_tests.sh
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include "../firmware/calcada_livre/logica.h"
using namespace cl;

static int falhas = 0, total = 0;
#define CHECK(cond, msg) do { total++; if (!(cond)) { falhas++; printf("  FALHOU: %s (linha %d)\n", msg, __LINE__); } } while (0)
#define TESTE(nome) printf("- %s\n", nome)

static const uint32_t DT = 60;   // intervalo entre leituras no firmware (ms)

struct Sim {
  Decisor d; uint32_t t = 1000; Saida ultima; Contadores c;
  explicit Sim(const Config& cfg = Config(), int ref = 800) : d(cfg) { d.calibrar(ref); }
  Saida passo(int dist, int luz, bool echo_alto = false) {
    Entrada e{dist, luz, echo_alto, t}; t += DT;
    Saida s = d.atualizar(e);
    c.registrar(ultima, s); ultima = s; return s;
  }
  Saida rep(int n, int dist, int luz, bool echo_alto = false) { Saida s; for (int i = 0; i < n; i++) s = passo(dist, luz, echo_alto); return s; }
};

int main() {
  TESTE("calibracao: limites relativos a luz do ambiente");
  { Decisor d; CHECK(d.calibrar(800), "ref 800 valida"); CHECK(d.referencia() == 800, "ref guardada");
    Saida s = d.atualizar({150, 800, false, 1000}); CHECK(s.limiar_escuro_entrar == 400 && s.limiar_escuro_sair == 520, "limites 400/520");
    Decisor d2; CHECK(d2.calibrar(300), "ref 300 valida"); s = d2.atualizar({150, 300, false, 1000});
    CHECK(s.limiar_escuro_entrar == 150 && s.limiar_escuro_sair == 195, "limites 150/195 em sala mais escura");
    Decisor d3; CHECK(!d3.calibrar(60), "ref 60 suspeita"); CHECK(d3.referencia() == 500, "usa referencia padrao"); }

  TESTE("caminho livre permanece LIVRE");
  { Sim s; Saida r = s.rep(50, 150, 800); CHECK(r.sit == LIVRE, "livre"); CHECK(s.c.obstaculos == 0 && s.c.trechos_escuros == 0, "sem eventos"); }

  TESTE("obstaculo a 15 cm e detectado em ate 3 leituras (180 ms)");
  { Sim s; s.rep(10, 150, 800); int n = 0; Saida r;
    do { r = s.passo(15, 800); n++; } while (r.sit != OBSTACULO && n < 10);
    CHECK(r.sit == OBSTACULO, "detectou"); CHECK(n <= 3, "latencia ate 3 leituras"); }

  TESTE("pico isolado do ultrassonico nao dispara (mediana)");
  { Sim s; s.rep(10, 150, 800); Saida r = s.passo(5, 800); CHECK(r.sit == LIVRE, "pico de 5 cm ignorado");
    r = s.rep(10, 150, 800); CHECK(r.sit == LIVRE, "continua livre"); CHECK(s.c.obstaculos == 0, "nao contou"); }

  TESTE("sem eco conta como longe (nao mascara obstaculo)");
  { Sim s(Config(), 800); Config c; c.sem_eco_falha_ms = 0; Sim s2(c, 800);
    s2.rep(10, -1, 800); Saida r = s2.rep(5, 12, 800); CHECK(r.sit == OBSTACULO, "obstaculo apos varias leituras sem eco"); }

  TESTE("histerese do obstaculo: 21-24 cm nao entra; so sai acima de 25 cm");
  { Sim s; s.rep(10, 150, 800);
    Saida r = s.rep(10, 22, 800); CHECK(r.sit == LIVRE, "22 cm nao entra");
    s.rep(6, 15, 800); r = s.rep(10, 23, 800); CHECK(r.sit == OBSTACULO, "23 cm mantem obstaculo");
    r = s.rep(10, 26, 800); CHECK(r.sit == LIVRE, "26 cm sai"); CHECK(s.c.obstaculos == 1, "contou uma vez"); }

  TESTE("trecho escuro: entra abaixo de 50% e sai acima de 65% da referencia");
  { Sim s(Config(), 800); s.rep(10, 150, 800);
    Saida r = s.rep(12, 150, 300); CHECK(r.sit == ESCURO, "300 < 400 entra");
    r = s.rep(12, 150, 450); CHECK(r.sit == ESCURO, "450 ainda escuro (entre 400 e 520)");
    r = s.rep(12, 150, 600); CHECK(r.sit == LIVRE, "600 > 520 sai"); CHECK(s.c.trechos_escuros == 1, "contou um trecho"); }

  TESTE("luz oscilando perto do limite nao faz o robo alternar");
  { Sim s(Config(), 800); s.rep(10, 150, 800); s.rep(12, 150, 300); int trocas = 0; Situacao ant = ESCURO;
    for (int i = 0; i < 40; i++) { Saida r = s.passo(150, 440 + (i % 2) * 70); if (r.sit != ant) { trocas++; ant = r.sit; } }
    CHECK(trocas == 0, "nenhuma troca com luz entre 440 e 510"); }

  TESTE("mesma logica em outra iluminacao (referencia 300)");
  { Sim s(Config(), 300); Saida r = s.rep(20, 150, 280); CHECK(r.sit == LIVRE, "280 e normal numa sala escura");
    r = s.rep(20, 150, 100); CHECK(r.sit == ESCURO, "100 e escuro nessa sala"); }

  TESTE("obstaculo no escuro = RISCO_ALTO; normaliza quando o obstaculo sai");
  { Sim s(Config(), 800); s.rep(10, 150, 800); s.rep(12, 150, 300);
    Saida r = s.rep(6, 12, 300); CHECK(r.sit == RISCO_ALTO, "risco alto"); CHECK(s.c.riscos_altos == 1, "contou risco");
    r = s.rep(10, 150, 300); CHECK(r.sit == ESCURO, "volta a ESCURO sem obstaculo");
    r = s.rep(12, 150, 700); CHECK(r.sit == LIVRE, "volta a LIVRE com luz"); }

  TESTE("contadores: 2 obstaculos, 1 trecho escuro, 1 risco alto");
  { Sim s(Config(), 800); s.rep(10, 150, 800);
    s.rep(6, 15, 800); s.rep(10, 150, 800);             // obstaculo 1
    s.rep(6, 15, 800); s.rep(10, 150, 800);             // obstaculo 2
    s.rep(12, 150, 300);                                // entra no escuro
    s.rep(6, 12, 300); s.rep(10, 150, 300);             // risco alto
    s.rep(12, 150, 700);                                // sai do escuro
    CHECK(s.c.obstaculos == 2, "2 obstaculos"); CHECK(s.c.trechos_escuros == 1, "1 trecho escuro");
    CHECK(s.c.riscos_altos == 1, "1 risco alto"); CHECK(s.c.falhas == 0, "0 falhas"); }

  TESTE("falha: LDR desconectado (leitura 0 por 3 s) entra em modo seguro e recupera");
  { Sim s; s.rep(10, 150, 800); Saida r = s.rep(30, 150, 0); CHECK(r.sit == LIVRE || r.sit == ESCURO, "ainda nao e falha antes de 3 s");
    r = s.rep(30, 150, 0); CHECK(r.sit == FALHA && r.falha == LDR_DESCONECTADO, "falha LDR apos 3 s");
    r = s.rep(5, 150, 800); CHECK(r.sit == FALHA, "ainda em falha (debounce de recuperacao)");
    r = s.rep(30, 150, 800); CHECK(r.sit != FALHA, "recupera apos leituras normais"); CHECK(s.c.falhas == 1, "contou uma falha"); }

  TESTE("LDR coberto com a mao (leitura baixa mas > 1) NAO e falha");
  { Sim s; s.rep(10, 150, 800); Saida r = s.rep(100, 150, 12); CHECK(r.sit == ESCURO, "escuro, nao falha"); }

  TESTE("LDR desconectado nao gera falso trecho escuro antes de virar falha");
  { Sim s; s.rep(10, 150, 800); Saida r = s.rep(40, 150, 0);
    CHECK(r.sit == LIVRE, "continua LIVRE enquanto o LDR esta suspeito"); CHECK(s.c.trechos_escuros == 0, "nao contou trecho escuro falso");
    r = s.rep(30, 150, 0); CHECK(r.sit == FALHA, "depois de 3 s vira falha"); }

  TESTE("falha: Echo preso em nivel alto");
  { Sim s; s.rep(5, 150, 800); Saida r = s.rep(3, 150, 800, true); CHECK(r.sit == FALHA && r.falha == ECHO_PRESO, "echo preso"); }

  TESTE("falha: sem eco por mais de 6 s (quando ativada); desligada por padrao");
  { Config c; c.sem_eco_falha_ms = 6000; Sim s(c); s.rep(10, 150, 800); Saida r = s.rep(105, -1, 800);
    CHECK(r.sit == FALHA && r.falha == SEM_ECO_LONGO, "ativada: sem eco longo vira falha");
    Sim s2; s2.rep(10, 150, 800); r = s2.rep(300, -1, 800); CHECK(r.sit == LIVRE, "padrao: ambiente aberto nao e falha"); }

  TESTE("filtros: mediana e media");
  { Filtro f; f.iniciar(5); int v[] = {10, 200, 12, 11, 13}; for (int x : v) f.add(x); CHECK(f.mediana() == 12, "mediana 12");
    Filtro g; g.iniciar(4); for (int x : {100, 200, 300, 400}) g.add(x); CHECK(g.media() == 250, "media 250"); }

  printf("\n%d verificacoes, %d falha(s)\n", total, falhas);
  return falhas ? 1 : 0;
}
