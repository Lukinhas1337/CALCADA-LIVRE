// Simula o firmware completo no PC: relogio e sensores controlados, pinos e serial observados.
//   bash tests/run_tests.sh
#define LEDS_COMPLETOS 1   // este teste verifica o modo de 3 LEDs; o modo minimo esta em test_firmware_minimo.cpp
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
static bool motoresParados() { return mundo.pwm[ENA] == 0 && mundo.pwm[ENB] == 0; }
static bool andandoParaFrente(int vel) {
  return mundo.pino[IN1] == HIGH && mundo.pino[IN2] == LOW && mundo.pino[IN3] == HIGH && mundo.pino[IN4] == LOW &&
         mundo.pwm[ENA] == vel && mundo.pwm[ENB] == vel;
}

int main() {
  setup();

  TESTE("calibracao ao ligar (luz do ambiente = 800)");
  avancar(3000, 150, 800);
  CHECK(tem("C,800,400,520"), "limites calculados 400/520");
  CHECK(modo == M_PATRULHA, "vai para PATRULHA");

  TESTE("caminho livre: anda em velocidade normal e LED verde");
  avancar(1000, 150, 800);
  CHECK(andandoParaFrente(VEL_NORMAL), "motores para frente a 255");
  CHECK(mundo.pino[LED_VERDE] == HIGH && mundo.pino[LED_AMARELO] == LOW && mundo.pino[LED_VERMELHO] == LOW, "so o LED verde");

  TESTE("obstaculo: para, bipa, LED vermelho, da re, gira e volta a patrulhar");
  avancar(400, 15, 800);
  CHECK(tem("E,") && tem(",OBSTACULO,"), "evento OBSTACULO na serial");
  CHECK(modo == M_MANOBRA || modo == M_PATRULHA, "entrou em manobra");
  avancar(200, 15, 800);
  CHECK(mundo.pino[LED_VERMELHO] == HIGH, "LED vermelho aceso na manobra");
  avancar(1800, 150, 800);                       // obstaculo saiu da frente depois do giro
  CHECK(modo == M_PATRULHA, "voltou a patrulhar");
  avancar(500, 150, 800);
  CHECK(andandoParaFrente(VEL_NORMAL), "voltou a andar normal");

  TESTE("trecho escuro: reduz a velocidade e acende o LED amarelo");
  avancar(1500, 150, 300);
  CHECK(tem(",ESCURO,"), "evento ESCURO");
  CHECK(andandoParaFrente(VEL_LENTA), "velocidade reduzida (190)");
  CHECK(mundo.pino[LED_AMARELO] == HIGH && mundo.pino[LED_VERDE] == LOW, "LED amarelo");

  TESTE("obstaculo no escuro: para, alarme continuo e so volta quando normaliza");
  avancar(700, 12, 300);
  CHECK(tem(",RISCO_ALTO,"), "evento RISCO_ALTO");
  CHECK(modo == M_ESPERA_RISCO, "modo ESPERA_RISCO");
  CHECK(motoresParados(), "motores parados");
  CHECK(mundo.pino[BUZZER] == HIGH, "buzzer em alarme continuo");
  avancar(1500, 150, 300);                        // obstaculo retirado
  CHECK(modo == M_PATRULHA, "voltou a patrulhar quando normalizou");
  CHECK(andandoParaFrente(VEL_LENTA), "segue devagar (ainda no escuro)");
  avancar(2000, 150, 800);
  CHECK(andandoParaFrente(VEL_NORMAL), "luz voltou: velocidade normal");

  TESTE("modo seguro: LDR desconectado para o robo e avisa; recupera sozinho");
  avancar(3500, 150, 0);
  CHECK(modo == M_SEGURO, "entrou em MODO SEGURO");
  CHECK(motoresParados(), "motores parados no modo seguro");
  CHECK(tem(",FALHA,"), "evento FALHA");
  avancar(2500, 150, 800);
  CHECK(modo == M_PATRULHA, "saiu do modo seguro com leituras normais");
  CHECK(tem(",FALHA_OK,"), "evento FALHA_OK");

  TESTE("modo seguro: Echo solto (pull-up le alto)");
  avancar(600, 150, 800, true);
  CHECK(modo == M_SEGURO, "echo preso -> modo seguro");
  avancar(2500, 150, 800, false);
  CHECK(modo == M_PATRULHA, "recuperou");

  TESTE("fim da rota: para e imprime o relatorio uma unica vez");
  avancar(60000, 150, 800);
  CHECK(modo == M_FIM, "modo FIM");
  CHECK(motoresParados(), "motores parados no fim");
  CHECK(tem("RELATORIO DA ROTA: 1 obstaculo(s), 1 trecho(s) escuro(s), 1 risco(s) alto(s), 2 falha(s)"), "relatorio com os contadores");
  size_t a = mundo.serial.find("R,"), b = mundo.serial.rfind("\nR,");
  CHECK(a != std::string::npos && (b == std::string::npos || b < a + 1), "linha R, impressa uma vez");

  printf("\n%d verificacoes, %d falha(s)\n", total, falhas);
  return falhas ? 1 : 0;
}
