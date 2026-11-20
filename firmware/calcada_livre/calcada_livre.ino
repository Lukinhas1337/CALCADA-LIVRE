// Calcada Livre - carrinho inspetor de calcada (Arduino Uno + L298N + HC-SR04 + LDR)
// Global Solution 2026.2 - Project-Based Maker Lab
//
// O que faz: patrulha uma rota, detecta OBSTACULO (ultrassonico) e TRECHO ESCURO (LDR),
// reage sozinho (para, desvia, reduz velocidade) e avisa o cidadao com buzzer e LED.
// Padrao: LDR + resistor 10k + buzzer ativo + 3 LEDs (verde, amarelo, vermelho). Nada impresso em 3D.
// Sem os LEDs externos: LEDS_COMPLETOS 0 usa so o LED da placa.
// A decisao fica em logica.h (testada no PC). Aqui ficam sensores, motores e avisos.
//
// Serial 9600:
//   C,luz_ref,limiar_entrar,limiar_sair        calibracao
//   D,t_ms,dist_cm,luz,situacao,modo           leitura (a cada LOG_INTERVALO_MS)
//   E,t_ms,TIPO,dist_cm,luz                    evento (OBSTACULO, ESCURO, RISCO_ALTO, FALHA, FALHA_OK)
//   R,obstaculos,trechos_escuros,riscos_altos,falhas,duracao_s   relatorio final

#include "logica.h"

// ---------- opcoes de compilacao ----------
#ifndef LEDS_COMPLETOS
#define LEDS_COMPLETOS 1        // 1 = 3 LEDs (verde D13, amarelo D9, vermelho D8). 0 = so o LED da placa (D13)
#endif
#define MOTORES_ATIVOS 1        // 0 = ensaio de bancada: nao anda, so sente e avisa
#define LOG_DETALHADO 1         // 1 = imprime as linhas D,... (util nos testes)
const unsigned long DURACAO_ROTA_S = 60;   // fim da rota: para e imprime o relatorio

// ---------- pinos (fiacao real do grupo) ----------
const int IN1 = 12, IN2 = 10;   // motores da esquerda
const int IN3 = 2,  IN4 = 4;    // motores da direita
const int ENA = 11, ENB = 3;    // velocidade (PWM)
const int TRIG = 6, ECHO = 7;   // HC-SR04
const int LDR_PIN = A0;         // 5V - LDR - A0 - 10k - GND (mais luz = leitura maior)
const int LED_UNICO = 13;                                       // so se LEDS_COMPLETOS 0: LED da propria placa
const int LED_VERDE = 13, LED_AMARELO = 9, LED_VERMELHO = 8;   // resistor de 220 ohm em serie com cada LED
const int BUZZER = 5;           // buzzer ATIVO ligado por digitalWrite. NAO use tone():
                                // ele usa o Timer2 e desliga o PWM dos pinos 3 e 11 (motores)

// ---------- velocidades e tempos ----------
const int VEL_NORMAL = 255;
const int VEL_LENTA  = 190;     // trecho escuro: ajuste ate andar devagar SEM travar
const int VEL_GIRO   = 255;
const unsigned long T_PARADA_MS = 150, T_RE_MS = 400, T_PAUSA_MS = 100, T_GIRO_MS = 500;
const unsigned long RISCO_TIMEOUT_MS = 8000;   // risco alto sem sair: tenta desviar
const unsigned long LEITURA_INTERVALO_MS = 60; // o HC-SR04 pede pelo menos ~50 ms entre disparos
const unsigned long LOG_INTERVALO_MS = 500;
const unsigned long CALIBRACAO_MS = 2500;

using namespace cl;
Decisor decisor;
Contadores contadores;
Saida saida, saidaAnterior;

enum Modo : uint8_t { M_CALIBRANDO, M_PATRULHA, M_MANOBRA, M_ESPERA_RISCO, M_SEGURO, M_FIM };
enum Fase : uint8_t { F_PARADA, F_RE, F_PAUSA, F_GIRO };
const char* NOMES_MODO[] = {"CALIBRANDO", "PATRULHA", "MANOBRA", "ESPERA_RISCO", "SEGURO", "FIM"};

Modo modo = M_CALIBRANDO;
Fase fase = F_PARADA;
unsigned long tModo = 0, tFase = 0, tUltimaLeitura = 0, tUltimoLog = 0, tInicioRota = 0, tCalib = 0;
long somaLuz = 0; int nLuz = 0;
bool girarDireita = true, relatorioFeito = false;

// ---------- buzzer sem bloqueio: lista de tempos {liga, desliga, liga, ...} em ms ----------
struct Padrao { const unsigned int* t; uint8_t n; bool repete; };
const unsigned int P_CURTO[]  = {80};                    // trecho escuro: 1 bip curto
const unsigned int P_DOIS[]   = {120, 120, 120};         // obstaculo: 2 bips
const unsigned int P_CONT[]   = {60000};                 // risco alto: alarme continuo
const unsigned int P_FALHA[]  = {100, 100, 100, 100, 100, 1500};   // falha: 3 bips e pausa
const unsigned int P_FIM[]    = {700};                   // fim da rota: 1 bip longo
const unsigned int P_AVISO[]  = {100, 100, 100, 100, 100};         // calibracao suspeita
const Padrao PAD_CURTO = {P_CURTO, 1, false}, PAD_DOIS = {P_DOIS, 3, false}, PAD_CONT = {P_CONT, 1, true},
             PAD_FALHA = {P_FALHA, 6, true}, PAD_FIM = {P_FIM, 1, false}, PAD_AVISO = {P_AVISO, 5, false};
const Padrao* padraoAtual = nullptr;
uint8_t idxPadrao = 0; unsigned long tPadrao = 0;

void tocar(const Padrao* p) {
  if (padraoAtual == p && p->repete) return;            // ja tocando o padrao repetitivo
  padraoAtual = p; idxPadrao = 0; tPadrao = millis(); digitalWrite(BUZZER, HIGH);
}
void silenciar() { padraoAtual = nullptr; digitalWrite(BUZZER, LOW); }
void atualizarBuzzer(unsigned long agora) {
  if (!padraoAtual) return;
  if (agora - tPadrao < padraoAtual->t[idxPadrao]) return;
  idxPadrao++; tPadrao = agora;
  if (idxPadrao >= padraoAtual->n) {
    if (padraoAtual->repete) { idxPadrao = 0; digitalWrite(BUZZER, HIGH); } else silenciar();
    return;
  }
  digitalWrite(BUZZER, (idxPadrao % 2 == 0) ? HIGH : LOW);
}

// ---------- LEDs ----------
#if LEDS_COMPLETOS
void leds(bool v, bool a, bool r) { digitalWrite(LED_VERDE, v); digitalWrite(LED_AMARELO, a); digitalWrite(LED_VERMELHO, r); }
#else
// Versao minima, um LED so: apagado = livre; aceso fixo = escuro ou obstaculo; piscando = risco alto, falha ou fim.
// O buzzer diferencia cada situacao (1 bip, 2 bips, continuo, 3 bips, bip longo).
void leds(bool v, bool a, bool r) { digitalWrite(LED_UNICO, (a || r || (modo == M_FIM && v)) ? HIGH : LOW); }
#endif
bool piscar(unsigned long agora, unsigned long meio) { return (agora / meio) % 2 == 0; }

// ---------- motores ----------
void motorEsq(int v) {
  if (v > 0)      { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
  else if (v < 0) { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); }
  else            { digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW); }
  analogWrite(ENA, abs(v));
}
void motorDir(int v) {
  if (v > 0)      { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
  else if (v < 0) { digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); }
  else            { digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW); }
  analogWrite(ENB, abs(v));
}
void parar()           { motorEsq(0); motorDir(0); }
void andar(int vel)    { if (MOTORES_ATIVOS) { motorEsq(vel); motorDir(vel); } else parar(); }
void re()              { if (MOTORES_ATIVOS) { motorEsq(-VEL_NORMAL); motorDir(-VEL_NORMAL); } else parar(); }
void girar(bool dir)   { if (MOTORES_ATIVOS) { if (dir) { motorEsq(VEL_GIRO); motorDir(-VEL_GIRO); } else { motorEsq(-VEL_GIRO); motorDir(VEL_GIRO); } } else parar(); }

// ---------- sensores ----------
int lerDistancia(bool &echoAltoAntes) {
  echoAltoAntes = (digitalRead(ECHO) == HIGH);           // em repouso o Echo deve estar em nivel baixo
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long dur = pulseIn(ECHO, HIGH, 25000);
  if (dur == 0) return -1;                               // sem eco
  return (int)(dur * 0.0343 / 2);
}
int lerLuz() { return analogRead(LDR_PIN); }

// ---------- registro ----------
void evento(const char* tipo, unsigned long t) {
  Serial.print(F("E,")); Serial.print(t); Serial.print(','); Serial.print(tipo); Serial.print(',');
  Serial.print(saida.dist_filtrada); Serial.print(','); Serial.println(saida.luz_filtrada);
}
void relatorio() {
  unsigned long dur = (millis() - tInicioRota) / 1000;
  Serial.print(F("R,")); Serial.print(contadores.obstaculos); Serial.print(',');
  Serial.print(contadores.trechos_escuros); Serial.print(','); Serial.print(contadores.riscos_altos); Serial.print(',');
  Serial.print(contadores.falhas); Serial.print(','); Serial.println(dur);
  Serial.print(F("RELATORIO DA ROTA: ")); Serial.print(contadores.obstaculos); Serial.print(F(" obstaculo(s), "));
  Serial.print(contadores.trechos_escuros); Serial.print(F(" trecho(s) escuro(s), "));
  Serial.print(contadores.riscos_altos); Serial.print(F(" risco(s) alto(s), "));
  Serial.print(contadores.falhas); Serial.print(F(" falha(s) em ")); Serial.print(dur); Serial.println(F(" s"));
}

void entrarModo(Modo m, unsigned long agora) {
  modo = m; tModo = agora;
  if (m == M_MANOBRA) { fase = F_PARADA; tFase = agora; parar(); }
  if (m == M_ESPERA_RISCO || m == M_SEGURO || m == M_FIM) parar();
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT_PULLUP);    // pull-up: Echo solto le ALTO e vira falha
#if LEDS_COMPLETOS
  pinMode(LED_VERDE, OUTPUT); pinMode(LED_AMARELO, OUTPUT); pinMode(LED_VERMELHO, OUTPUT);
#else
  pinMode(LED_UNICO, OUTPUT);
#endif
  pinMode(BUZZER, OUTPUT);
  parar(); silenciar(); leds(false, false, false);
  Serial.println(F("Calcada Livre - iniciando. Deixe o sensor de luz em ambiente CLARO para calibrar."));
  tCalib = millis();
}

void loop() {
  unsigned long agora = millis();
  atualizarBuzzer(agora);

  // ----- calibracao: mede a luz do ambiente e define o limite de "escuro" -----
  if (modo == M_CALIBRANDO) {
    leds(false, piscar(agora, 200), false);
    if (agora - tUltimaLeitura >= 50) { somaLuz += lerLuz(); nLuz++; tUltimaLeitura = agora; }
    if (agora - tCalib >= CALIBRACAO_MS) {
      int ref = nLuz ? (int)(somaLuz / nLuz) : 0;
      bool ok = decisor.calibrar(ref);
      saida = decisor.atualizar({-1, ref, false, (uint32_t)agora});     // so para ler os limites
      Serial.print(F("C,")); Serial.print(decisor.referencia()); Serial.print(',');
      Serial.print(saida.limiar_escuro_entrar); Serial.print(','); Serial.println(saida.limiar_escuro_sair);
      if (!ok) { Serial.println(F("AVISO: luz de referencia muito baixa. Reinicie em ambiente claro. Usando valor padrao.")); tocar(&PAD_AVISO); }
      decisor = Decisor(); decisor.calibrar(ref);              // zera os filtros para a rota
      saida = Saida(); saidaAnterior = Saida();
      tInicioRota = agora; tUltimaLeitura = 0; tUltimoLog = agora;
      entrarModo(M_PATRULHA, agora);
    }
    return;
  }

  // ----- leitura e decisao a cada 60 ms -----
  if (agora - tUltimaLeitura >= LEITURA_INTERVALO_MS && modo != M_FIM) {
    tUltimaLeitura = agora;
    bool echoAlto = false;
    int d = lerDistancia(echoAlto);
    saidaAnterior = saida;
    saida = decisor.atualizar({d, lerLuz(), echoAlto, (uint32_t)agora});
    contadores.registrar(saidaAnterior, saida);

    if (saida.sit != saidaAnterior.sit) {                      // evento na mudanca de situacao
      if (saida.sit == FALHA) evento("FALHA", agora);
      else if (saidaAnterior.sit == FALHA) evento("FALHA_OK", agora);
      else if (saida.sit == RISCO_ALTO) evento("RISCO_ALTO", agora);
      else if (saida.sit == OBSTACULO && !saidaAnterior.obstaculo) evento("OBSTACULO", agora);
      else if (saida.escuro && !saidaAnterior.escuro) evento("ESCURO", agora);
      // avisos sonoros de entrada
      if (saida.sit == OBSTACULO && !saidaAnterior.obstaculo) tocar(&PAD_DOIS);
      else if (saida.sit == ESCURO && !saidaAnterior.escuro) tocar(&PAD_CURTO);
    }
#if LOG_DETALHADO
    if (agora - tUltimoLog >= LOG_INTERVALO_MS) {
      tUltimoLog = agora;
      Serial.print(F("D,")); Serial.print(agora); Serial.print(','); Serial.print(saida.dist_filtrada); Serial.print(',');
      Serial.print(saida.luz_filtrada); Serial.print(','); Serial.print(nome(saida.sit)); Serial.print(',');
      Serial.println(NOMES_MODO[modo]);
    }
#endif
  }

  // ----- modo seguro tem prioridade sobre tudo -----
  if (saida.sit == FALHA && modo != M_SEGURO && modo != M_FIM) { entrarModo(M_SEGURO, agora); tocar(&PAD_FALHA); }

  // ----- fim da rota -----
  if (modo != M_FIM && modo != M_SEGURO && (agora - tInicioRota) >= DURACAO_ROTA_S * 1000UL) {
    entrarModo(M_FIM, agora); silenciar(); tocar(&PAD_FIM);
  }

  // ----- maquina de estados -----
  switch (modo) {
    case M_PATRULHA:
      if (saida.sit == RISCO_ALTO) { entrarModo(M_ESPERA_RISCO, agora); tocar(&PAD_CONT); break; }
      if (saida.sit == OBSTACULO)  { entrarModo(M_MANOBRA, agora); break; }
      if (saida.sit == ESCURO)     { andar(VEL_LENTA); leds(false, true, false); }     // reduz velocidade, LED amarelo
      else                         { andar(VEL_NORMAL); leds(true, false, false); }    // livre: LED verde
      break;

    case M_MANOBRA:                                   // para -> re -> pausa -> gira -> volta a patrulhar
      leds(false, false, true);
      if (fase == F_PARADA && agora - tFase >= T_PARADA_MS) { fase = F_RE; tFase = agora; re(); }
      else if (fase == F_RE && agora - tFase >= T_RE_MS)    { fase = F_PAUSA; tFase = agora; parar(); }
      else if (fase == F_PAUSA && agora - tFase >= T_PAUSA_MS) { fase = F_GIRO; tFase = agora; girar(girarDireita); }
      else if (fase == F_GIRO && agora - tFase >= T_GIRO_MS) { parar(); girarDireita = !girarDireita; entrarModo(M_PATRULHA, agora); }
      break;

    case M_ESPERA_RISCO:                              // obstaculo no escuro: para, alarme continuo, LED vermelho piscando
      parar(); leds(false, false, piscar(agora, 125));
      if (saida.sit != RISCO_ALTO) { silenciar(); entrarModo(M_PATRULHA, agora); }          // situacao normalizou
      else if (agora - tModo >= RISCO_TIMEOUT_MS) { silenciar(); entrarModo(M_MANOBRA, agora); }  // nao saiu: tenta desviar
      break;

    case M_SEGURO:                                    // leitura impossivel: nao segue "cego"
      parar(); { bool p = piscar(agora, 250); leds(p, p, p); }
      if (saida.sit != FALHA) { silenciar(); entrarModo(M_PATRULHA, agora); }
      break;

    case M_FIM:
      parar(); leds(piscar(agora, 500), false, false);
      if (!relatorioFeito) { relatorio(); relatorioFeito = true; }
      break;

    default: break;
  }
}
