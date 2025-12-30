// ESCANEO DE ZONA

// === Pines del robot ===
const int AIN1 = 2;
const int AIN2 = 3;
const int BIN1 = 5;
const int BIN2 = 4;

// === Pines de sensores Sharp ===
const int SENSOR_IZQ = A7;
const int SENSOR_DER = A6;

// === Umbral de distancia en cm ===
const int UMBRAL = 15;
const int VELOCIDAD = 145;
const int VELOCIDAD_GIRO = 255;

// === Variables de distancias ===
float distIzq, distDer;

// Calibración sensores
const float a1 = 201429.7845;
const float b1 = -1.2409;
const float a2 = 890559.3060;
const float b2 = -1.4085;

void setup() {
  analogReadResolution(12);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  distIzq = leerDistancia(SENSOR_IZQ);
  distDer = leerDistancia(SENSOR_DER);

  if (distIzq < UMBRAL && distDer < UMBRAL) {
  escaneoZona(); 
  } else {
    detenerMotores();
}
  delay(10); 
}

// === Movimientos ===
void avanzar() {
  analogWrite(AIN1, VELOCIDAD);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, VELOCIDAD);
  analogWrite(BIN2, 0);
}

void retroceder() {
  analogWrite(AIN1, 0);
  analogWrite(AIN2, VELOCIDAD_GIRO);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, VELOCIDAD_GIRO);
}

void girarIzquierda() {
  analogWrite(AIN1, 0);
  analogWrite(AIN2, VELOCIDAD_GIRO);
  analogWrite(BIN1, VELOCIDAD_GIRO);
  analogWrite(BIN2, 0);
}

void girarDerecha() {
  analogWrite(AIN1, VELOCIDAD_GIRO);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, VELOCIDAD_GIRO);
}

void detenerMotores() {
  analogWrite(AIN1, 0);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, 0);
}

void escaneoZona() {
  // retroceder un poco
  retroceder();
  delay(500);
  detenerMotores();
  delay(100);

  // girar ligeramente a la izquierda y leer
  girarIzquierda();
  delay(300);
  detenerMotores();
  delay(100);
  float izquierdaEscaneo = leerDistancia(SENSOR_IZQ) + leerDistancia(SENSOR_DER);

  // girar ligeramente a la derecha y leer
  girarDerecha();
  delay(600); // gira más para volver al centro + derecha
  detenerMotores();
  delay(100);
  float derechaEscaneo = leerDistancia(SENSOR_IZQ) + leerDistancia(SENSOR_DER);

  // elegir dirección con más espacio
  if (izquierdaEscaneo > derechaEscaneo) {
    girarIzquierda();
    delay(400);
    avanzar();
    delay(1000);
  } else {
    girarDerecha();
    delay(400);
    avanzar();
    delay(1000);
  }

  detenerMotores();
}

// === Lectura rápida de sensor Sharp ===
float leerDistancia(int pin) {
  const int n = 5; // más rápido
  long sum = 0;
  for (int i = 0; i < n; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  float adc = sum / float(n);
  if (pin == SENSOR_IZQ) {
    return a1 * pow(adc, b1);
  } else {
    return a2 * pow(adc, b2);
  }
}
