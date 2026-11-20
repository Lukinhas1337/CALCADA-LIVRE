// TESTE DAS PECAS NOVAS: 3 LEDs, buzzer e LDR. Sem motores. Use so com o cabo USB.
// Abra o Monitor Serial em 9600. Repete em ciclo.
//   1) acende cada LED por ~1 s, com um bip curto no inicio
//   2) imprime 12 leituras do LDR: cubra o sensor com a mao e veja o valor CAIR

const int LED_VERDE = 13, LED_AMARELO = 9, LED_VERMELHO = 8;
const int BUZZER = 5;
const int LDR_PIN = A0;

void acender(int pino, const char* nome) {
  Serial.print(F("LED "));
  Serial.println(nome);
  digitalWrite(pino, HIGH);
  digitalWrite(BUZZER, HIGH); delay(80); digitalWrite(BUZZER, LOW);
  delay(620);
  digitalWrite(pino, LOW);
  delay(300);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_VERDE, OUTPUT); pinMode(LED_AMARELO, OUTPUT); pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println(F("Teste das pecas novas. Veja os LEDs, ouca os bips e acompanhe o LDR."));
}

void loop() {
  acender(LED_VERDE, "verde");
  acender(LED_AMARELO, "amarelo");
  acender(LED_VERMELHO, "vermelho");
  Serial.println(F("--- LDR: cubra com a mao e veja o valor cair ---"));
  for (int i = 0; i < 12; i++) {
    Serial.print(F("LDR="));
    Serial.println(analogRead(LDR_PIN));
    delay(250);
  }
  Serial.println(F("--- repetindo ---"));
}
