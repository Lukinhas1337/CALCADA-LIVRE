// Versao MINIMA do firmware (LDR + buzzer + LED da placa): so o D13 e usado como LED.
#define LEDS_COMPLETOS 0
#include "mock/Arduino.h"
#include "../firmware/calcada_livre/calcada_livre.ino"
#include <cstdio>

static int falhas = 0, total = 0;
#define CHECK(cond, msg) do { total++; if (!(cond)) { falhas++; printf("  FALHOU: %s (linha %d)\n", msg, __LINE__); } } while (0)
#define TESTE(nome) printf("- %s\n", nome)

static void avancar(unsigned long ms, int dist, int luz, bool echo_alto = false) {
  mundo.dist_cm = dist; mundo.luz = luz; mundo.echo_alto = echo_alto;
  unsigned long fim = mundo.ms + ms;
  while (mundo.ms < fim) { mundo.ms += 5; loop(); }
}
static bool tem(const char* t) { return mundo.serial.find(t) != std::string::npos; }
// observa o LED durante `ms` e diz se ele ficou sempre aceso, sempre apagado ou piscou
static int observarLed(unsigned long ms, int dist, int luz) {
  bool viuAceso = false, viuApagado = false; unsigned long fim = mundo.ms + ms;
  mundo.dist_cm = dist; mundo.luz = luz;
  while (mundo.ms < fim) { mundo.ms += 5; loop(); if (mundo.pino[LED_UNICO]) viuAceso = true; else viuApagado = true; }
  return viuAceso && viuApagado ? 2 : (viuAceso ? 1 : 0);   // 0 apagado, 1 aceso fixo, 2 piscando
}

int main() {
  setup();
  TESTE("calibracao e caminho livre: LED apagado, anda normal");
  avancar(3000, 150, 800);
  CHECK(tem("C,800,400,520"), "calibrou");
  CHECK(observarLed(800, 150, 800) == 0, "LED apagado em caminho livre");
  CHECK(mundo.pwm[ENA] == VEL_NORMAL && mundo.pwm[ENB] == VEL_NORMAL, "velocidade normal");

  TESTE("trecho escuro: LED aceso fixo, anda devagar, 1 bip");
  avancar(1500, 150, 300);
  CHECK(tem(",ESCURO,"), "evento ESCURO");
  CHECK(observarLed(600, 150, 300) == 1, "LED aceso fixo no escuro");
  CHECK(mundo.pwm[ENA] == VEL_LENTA, "velocidade reduzida");

  TESTE("obstaculo no escuro (risco alto): LED pisca, motores parados, buzzer continuo");
  avancar(700, 12, 300);
  CHECK(tem(",RISCO_ALTO,"), "evento RISCO_ALTO");
  CHECK(observarLed(600, 12, 300) == 2, "LED piscando em risco alto");
  CHECK(mundo.pwm[ENA] == 0 && mundo.pwm[ENB] == 0, "motores parados");
  CHECK(mundo.pino[BUZZER] == HIGH, "buzzer em alarme continuo");

  TESTE("modo seguro (LDR desconectado): LED pisca e motores param");
  avancar(2000, 150, 700);
  avancar(3600, 150, 0);
  CHECK(modo == M_SEGURO, "modo seguro");
  CHECK(observarLed(600, 150, 0) == 2, "LED piscando na falha");
  CHECK(mundo.pwm[ENA] == 0 && mundo.pwm[ENB] == 0, "motores parados");

  TESTE("os pinos de LED extras (D8 e D9) nao sao usados na versao minima");
  CHECK(mundo.pino[LED_VERDE] == 0 && mundo.pino[LED_AMARELO] == 0, "D8 e D9 nunca acionados");

  TESTE("fim da rota: LED pisca e relatorio sai");
  avancar(2500, 150, 800);
  avancar(60000, 150, 800);
  CHECK(modo == M_FIM, "modo FIM");
  CHECK(tem("RELATORIO DA ROTA:"), "relatorio impresso");

  printf("\n%d verificacoes, %d falha(s)\n", total, falhas);
  return falhas ? 1 : 0;
}
