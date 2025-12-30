// VAGANCIA SUAVE

// === Pines del robot ===
const int AIN1 = 2;
const int AIN2 = 3;
const int BIN1 = 5;
const int BIN2 = 4;

const int v_vago = 115;
const int v_vago_giro = 195;

void setup() {
  analogReadResolution(12);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  vagarAleatoriamente();

  delay(10);
}

// === Movimientos ===
void avanzar() {
  analogWrite(AIN1, v_vago);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, v_vago);
  analogWrite(BIN2, 0);
}

void girarIzquierda() {
  analogWrite(AIN1, 0);
  analogWrite(AIN2, v_vago_giro);
  analogWrite(BIN1, v_vago_giro);
  analogWrite(BIN2, 0);
}

void girarDerecha() {
  analogWrite(AIN1, v_vago_giro);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, v_vago_giro);
}

void detenerMotores() {
  analogWrite(AIN1, 0);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, 0);
}

// === Vagancia aleatoria suave ===
void vagarAleatoriamente() {
  avanzar();
  delay(random(500, 1000));  // Avanza un tiempo aleatorio
  detenerMotores();
  delay(100);
  if (random(0, 2) == 0) {
    girarIzquierda();
  } else {
    girarDerecha();
  }
  delay(random(300, 800)); // Gira un poco
  avanzar();
  delay(random(500, 1000));  // Avanza un tiempo aleatorio
  detenerMotores();
  delay(100);
}
