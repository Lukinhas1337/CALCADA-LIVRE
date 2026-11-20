// TESTE DE MOTORES - carrinho 4WD (Arduino Uno + L298N)
// Use com o carrinho LEVANTADO (rodas no ar).
// Acompanhe no Monitor Serial (9600 baud).

// Pinos conforme a fiacao informada
const int IN1 = 12, IN2 = 10;  // lado esquerdo
const int IN3 = 2,  IN4 = 4;   // lado direito
const int ENA = 11, ENB = 3;   // habilitacao (velocidade)

void paraTudo() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void passo(const char* nome, int i1, int i2, int i3, int i4, int ea, int eb) {
  Serial.println(nome);
  digitalWrite(LED_BUILTIN, HIGH);   // LED L aceso durante o passo
  digitalWrite(IN1, i1); digitalWrite(IN2, i2);
  digitalWrite(IN3, i3); digitalWrite(IN4, i4);
  analogWrite(ENA, ea);
  analogWrite(ENB, eb);
  delay(2000);
  paraTudo();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  paraTudo();
  Serial.println("Teste iniciado. Comeca em 3 segundos...");
  delay(3000);
}

void loop() {
  passo("1) ESQUERDA para frente", HIGH, LOW,  LOW,  LOW,  255, 0);
  passo("2) ESQUERDA para tras",   LOW,  HIGH, LOW,  LOW,  255, 0);
  passo("3) DIREITA para frente",  LOW,  LOW,  HIGH, LOW,  0,   255);
  passo("4) DIREITA para tras",    LOW,  LOW,  LOW,  HIGH, 0,   255);
  passo("5) TODOS para frente",    HIGH, LOW,  HIGH, LOW,  255, 255);
  Serial.println("--- fim da rodada, repetindo ---");
  delay(2000);
}
