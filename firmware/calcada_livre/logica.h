// logica.h - decisao do carrinho "Calcada Livre", SEM dependencia do Arduino.
// Por isso pode ser testada no PC (tests/test_logica.cpp) antes de gravar na placa.
//
// Entradas: distancia do HC-SR04 e leitura do LDR. Saida: uma de 5 situacoes.
//   LIVRE | OBSTACULO | ESCURO | RISCO_ALTO (obstaculo no escuro) | FALHA (modo seguro)
#pragma once
#include <stdint.h>

namespace cl {

struct Config {
  // --- obstaculo: histerese de 5 cm para o robo nao ficar "piscando" no limite ---
  int dist_entrar_cm = 20;      // entra em OBSTACULO ao chegar a 20 cm ou menos
  int dist_sair_cm   = 25;      // so sai quando passa de 25 cm
  // --- escuro: limites RELATIVOS a luz medida na calibracao (funciona em qualquer sala) ---
  float frac_escuro_entrar = 0.50f;   // escuro se a luz cair abaixo de 50% da referencia
  float frac_escuro_sair   = 0.65f;   // so volta ao claro acima de 65% da referencia
  int   luz_ref_minima     = 150;     // referencia menor que isso: calibracao suspeita
  int   luz_ref_padrao     = 500;     // usada se a calibracao for suspeita
  // --- filtros ---
  uint8_t janela_dist = 5;      // mediana das ultimas 5 leituras do ultrassonico
  uint8_t janela_luz  = 8;      // media das ultimas 8 leituras do LDR
  // --- falhas (modo seguro) ---
  int      ldr_falha_baixo = 1;       // leitura <= 1 por muito tempo = LDR desconectado
  int      ldr_falha_alto  = 1022;    // leitura >= 1022 por muito tempo = curto/fio solto
  uint32_t ldr_falha_ms    = 3000;
  uint8_t  echo_preso_leituras = 3;   // pino Echo ja em nivel alto antes do disparo
  uint32_t sem_eco_falha_ms = 0;      // 0 = DESLIGADO (padrao): em ambiente aberto nao ter eco e normal.
                                      // Em sala fechada pode usar 6000 (6 s sem eco = sensor sem resposta)
  uint32_t recuperacao_ms   = 1000;   // leituras normais por esse tempo para sair da falha
};

enum Situacao : uint8_t { LIVRE = 0, OBSTACULO = 1, ESCURO = 2, RISCO_ALTO = 3, FALHA = 4 };
enum MotivoFalha : uint8_t { SEM_FALHA = 0, LDR_DESCONECTADO = 1, ECHO_PRESO = 2, SEM_ECO_LONGO = 3 };

inline const char* nome(Situacao s) {
  switch (s) {
    case LIVRE: return "LIVRE";
    case OBSTACULO: return "OBSTACULO";
    case ESCURO: return "ESCURO";
    case RISCO_ALTO: return "RISCO_ALTO";
    default: return "FALHA";
  }
}

// Janela deslizante com mediana e media
struct Filtro {
  int buf[16];
  uint8_t tam = 5, n = 0, pos = 0;
  void iniciar(uint8_t t) { tam = (t < 1) ? 1 : (t > 16 ? 16 : t); n = 0; pos = 0; }
  void add(int x) { buf[pos] = x; pos = (pos + 1) % tam; if (n < tam) n++; }
  int mediana() const {
    int v[16];
    for (uint8_t i = 0; i < n; i++) v[i] = buf[i];
    for (uint8_t i = 1; i < n; i++) { int k = v[i]; int j = i - 1; while (j >= 0 && v[j] > k) { v[j + 1] = v[j]; j--; } v[j + 1] = k; }
    return n ? v[n / 2] : 0;
  }
  int media() const {
    long s = 0;
    for (uint8_t i = 0; i < n; i++) s += buf[i];
    return n ? (int)(s / n) : 0;
  }
};

struct Entrada {
  int      dist_cm;          // -1 = sem eco (nada ate ~4 m ou sensor sem resposta)
  int      luz_raw;          // 0..1023 (mais luz = valor maior)
  bool     echo_idle_alto;   // pino Echo estava em nivel alto ANTES do disparo
  uint32_t agora_ms;
};

struct Saida {
  Situacao    sit = LIVRE;
  MotivoFalha falha = SEM_FALHA;
  bool obstaculo = false, escuro = false;
  int  dist_filtrada = 999, luz_filtrada = 0;
  int  limiar_escuro_entrar = 0, limiar_escuro_sair = 0;
};

class Decisor {
 public:
  explicit Decisor(const Config& c = Config()) : cfg(c) {
    fd.iniciar(cfg.janela_dist);
    fl.iniciar(cfg.janela_luz);
  }

  // Define os limites de "escuro" a partir da luz do ambiente. Devolve false se a referencia
  // parece ruim (ex.: calibrou no escuro); nesse caso usa luz_ref_padrao.
  bool calibrar(int luz_ref) {
    bool ok = luz_ref >= cfg.luz_ref_minima;
    ref = ok ? luz_ref : cfg.luz_ref_padrao;
    lim_entrar = (int)(ref * cfg.frac_escuro_entrar);
    lim_sair   = (int)(ref * cfg.frac_escuro_sair);
    escuro = false;
    return ok;
  }
  int referencia() const { return ref; }

  Saida atualizar(const Entrada& e) {
    // 1) filtros: sem eco conta como "longe" (999) para nao mascarar obstaculo
    fd.add(e.dist_cm < 0 ? 999 : e.dist_cm);
    fl.add(e.luz_raw);

    // 2) falhas
    MotivoFalha f = detectarFalha(e);
    if (f != SEM_FALHA) { falha_ativa = true; motivo = f; t_normal = 0; }
    else if (falha_ativa) {
      if (t_normal == 0) t_normal = e.agora_ms ? e.agora_ms : 1;
      if (e.agora_ms - t_normal >= cfg.recuperacao_ms) { falha_ativa = false; motivo = SEM_FALHA; }
    }

    Saida s;
    s.dist_filtrada = fd.mediana();
    s.luz_filtrada = fl.media();
    s.limiar_escuro_entrar = lim_entrar;
    s.limiar_escuro_sair = lim_sair;
    if (falha_ativa) { s.sit = FALHA; s.falha = motivo; s.obstaculo = obstaculo; s.escuro = escuro; return s; }

    // 3) histerese do obstaculo
    int d = s.dist_filtrada;
    if (!obstaculo && d <= cfg.dist_entrar_cm) obstaculo = true;
    else if (obstaculo && d >= cfg.dist_sair_cm) obstaculo = false;

    // 4) histerese do escuro. Fica congelada enquanto o LDR esta "suspeito" (leitura extrema que
    //    ainda nao virou falha): sem isso, um LDR desconectado geraria um falso trecho escuro.
    int l = s.luz_filtrada;
    if (t_ldr == 0) {
      if (!escuro && l < lim_entrar) escuro = true;
      else if (escuro && l > lim_sair) escuro = false;
    }

    s.obstaculo = obstaculo;
    s.escuro = escuro;
    s.sit = (obstaculo && escuro) ? RISCO_ALTO : obstaculo ? OBSTACULO : escuro ? ESCURO : LIVRE;
    return s;
  }

 private:
  MotivoFalha detectarFalha(const Entrada& e) {
    // LDR em 0 ou 1023 por tempo suficiente. Escuro real, mesmo com a mao cobrindo, fica acima de 1.
    bool extremo = (e.luz_raw <= cfg.ldr_falha_baixo) || (e.luz_raw >= cfg.ldr_falha_alto);
    if (extremo) { if (t_ldr == 0) t_ldr = e.agora_ms ? e.agora_ms : 1; } else t_ldr = 0;
    // Echo preso em alto antes do disparo (fio solto com pull-up, ou sensor travado)
    if (e.echo_idle_alto) { if (echo_cont < 255) echo_cont++; } else echo_cont = 0;
    // Sem eco por muito tempo
    if (cfg.sem_eco_falha_ms && e.dist_cm < 0) { if (t_sem_eco == 0) t_sem_eco = e.agora_ms ? e.agora_ms : 1; } else t_sem_eco = 0;

    if (t_ldr && e.agora_ms - t_ldr >= cfg.ldr_falha_ms) return LDR_DESCONECTADO;
    if (echo_cont >= cfg.echo_preso_leituras) return ECHO_PRESO;
    if (t_sem_eco && e.agora_ms - t_sem_eco >= cfg.sem_eco_falha_ms) return SEM_ECO_LONGO;
    return SEM_FALHA;
  }

  Config cfg;
  Filtro fd, fl;
  int ref = 500, lim_entrar = 250, lim_sair = 325;
  bool obstaculo = false, escuro = false, falha_ativa = false;
  MotivoFalha motivo = SEM_FALHA;
  uint32_t t_ldr = 0, t_sem_eco = 0, t_normal = 0;
  uint8_t echo_cont = 0;
};

// Conta os eventos da rota (para o relatorio final)
struct Contadores {
  uint16_t obstaculos = 0, trechos_escuros = 0, riscos_altos = 0, falhas = 0;
  void registrar(const Saida& antes, const Saida& agora) {
    if (agora.sit == FALHA && antes.sit != FALHA) falhas++;
    if (agora.sit == FALHA) return;
    if (agora.escuro && !antes.escuro) trechos_escuros++;
    if (agora.obstaculo && !antes.obstaculo) {
      if (agora.escuro) riscos_altos++; else obstaculos++;
    } else if (agora.escuro && !antes.escuro && agora.obstaculo) {
      // entrou no escuro ja com obstaculo a frente: vira risco alto
      riscos_altos++; if (obstaculos) obstaculos--;
    }
  }
};

}  // namespace cl
