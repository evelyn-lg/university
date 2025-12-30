// === Pines del robot ===
const int AIN1       = 2;
const int AIN2       = 3;
const int BIN1       = 5;
const int BIN2       = 4;
// Asignamos correctamente los pines de sensor
const int SENSOR_IZQ = A7; // Sensor izquierdo debe ser A7
const int SENSOR_DER = A6; // Sensor derecho debe ser A6

// === Configuración de Lógica Difusa ===
const int N_INPUTS  = 2;
const int N_REGIONS = 3;
const int N_RULES   = 9;
const int SAMPLES   = 52;

const float OUT_MIN = 0;
const float OUT_MAX = 255;

// Funciones de membresía - Inputs: {Short, Med, Far}
const float IN_SHORT[4] = {0, 0, 8, 12};     // activación completa hasta 8 cm, decrece hasta 12
const float IN_MED[3]   = {10, 17, 25};      // pico en 17 cm, desde 10 a 25 cm
const float IN_FAR[4]   = {20, 28, 32, 35};  // empieza en 20 cm, completa en 28–32 cm

// Funciones de membresía - Outputs: {Slow, Med, High}
const float OUT_SLOW[3] = {150, 180, 210};
const float OUT_MED[3]  = {180, 210, 240};
const float OUT_HIGH[3] = {210, 240, 270};

// Tabla de reglas: {inIzq, inDer, outIzq, outDer}
const uint8_t RULES[N_RULES][4] = {
  {0, 0, 2, 2}, {1, 0, 2, 1}, {0, 1, 1, 2},
  {1, 1, 1, 1}, {1, 2, 0, 1}, {2, 1, 1, 0},
  {2, 2, 0, 0}, {0, 2, 0, 2}, {2, 0, 2, 0}
};

// === Variables globales ===
float distancias[2];
int pwmIzq = 0, pwmDer = 0;

// Parámetros calibración A7 (izq)
const float a1 = 201429.7845;
const float b1 = -1.2409;
// Parámetros calibración A6 (der)
const float a2 = 890559.3060;
const float b2 = -1.4085;

// === Prototipos ===
float readSharpCM(int pin);
void setMotor(int pin1, int pin2, int speed);
void fuzzify(const float xval[], float memb[][2]);
void fuzzyInference(float memb[][2], int &pwmL, int &pwmR);
float triangle(float a, float b, float c, float x);
float trapezoid(float a, float b, float c, float d, float x);

void setup() {
  analogReadResolution(12);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("=== Robot Fuzzy con Sensores Calibrados ===");
}

void loop() {
  // 1) Lectura calibrada de sensores
  distancias[0] = readSharpCM(SENSOR_IZQ);
  distancias[1] = readSharpCM(SENSOR_DER);

  //Verificar si no hay enemigo a la vista
  if (distancias[0] > 28 && distancias[1] > 30) {
    pwmIzq = 150;  // Giro para buscar
    pwmDer = 0;
  } else {
    // 2) Fuzzificación
    float memb[N_REGIONS][N_INPUTS];
    fuzzify(distancias, memb);

    // 3) Inferencia y defuzzificación
    fuzzyInference(memb, pwmIzq, pwmDer);
  }

  // 4) Limitar PWM
  pwmIzq = constrain(pwmIzq, 0, 320);
  pwmDer = constrain(pwmDer, 0, 320);

  // 5) Mostrar por Serial
  Serial.print("Dist (cm) Izq: "); Serial.print(distancias[0],2);
  Serial.print(" | Der: "); Serial.print(distancias[1],2);
  Serial.print("  -> PWM Izq: "); Serial.print(pwmIzq);
  Serial.print(" | Der: "); Serial.println(pwmDer);

  // 6) Control de motores
  setMotor(AIN1, AIN2, pwmIzq);
  setMotor(BIN1, BIN2, pwmDer);

  delay(50);
}

// Implementación de lectura calibrada
float readSharpCM(int pin) {
  const int n = 60;
  long sum = 0;
  for (int i = 0; i < n; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  float adc = sum / float(n);

  // Selección de coeficientes según pin
  if (pin == SENSOR_IZQ) {
    // Izquierdo A7
    return a1 * pow(adc, b1);
  } else {
    // Derecho A6
    return a2 * pow(adc, b2);
  }
}

void setMotor(int pin1, int pin2, int speed) {
  analogWrite(pin1, speed);
  digitalWrite(pin2, LOW);
}

void fuzzify(const float xval[], float memb[][2]) {
  for (int i = 0; i < N_INPUTS; i++) {
    memb[0][i] = trapezoid(IN_SHORT[0], IN_SHORT[1], IN_SHORT[2], IN_SHORT[3], xval[i]);
    memb[1][i] = triangle(IN_MED[0], IN_MED[1], IN_MED[2], xval[i]);
    memb[2][i] = trapezoid(IN_FAR[0], IN_FAR[1], IN_FAR[2], IN_FAR[3], xval[i]);
  }
}

float triangle(float a, float b, float c, float x) {
  if (x <= a || x >= c) return 0;
  if (x == b) return 1;
  return (x < b) ? (x - a) / (b - a) : (c - x) / (c - b);
}

float trapezoid(float a, float b, float c, float d, float x) {
  if (x <= a || x >= d) return 0;
  if (x >= b && x <= c) return 1;
  return (x < b) ? (x - a) / (b - a) : (d - x) / (d - c);
}

void fuzzyInference(float memb[][2], int &pwmL, int &pwmR) {
  float aggL[SAMPLES] = {0}, aggR[SAMPLES] = {0};
  for (int r = 0; r < N_RULES; r++) {
    float mu = min(memb[RULES[r][0]][0], memb[RULES[r][1]][1]);
    if (mu < 0.001) continue;
    uint8_t outL = RULES[r][2];
    uint8_t outR = RULES[r][3];
    for (int s = 0; s < SAMPLES; s++) {
      float x = OUT_MIN + s * (OUT_MAX - OUT_MIN) / (SAMPLES - 1);
      float mvals[3] = {
        triangle(OUT_SLOW[0], OUT_SLOW[1], OUT_SLOW[2], x),
        triangle(OUT_MED[0],  OUT_MED[1],  OUT_MED[2],  x),
        triangle(OUT_HIGH[0], OUT_HIGH[1], OUT_HIGH[2], x)
      };
      aggL[s] = max(aggL[s], min(mu, mvals[outL]));
      aggR[s] = max(aggR[s], min(mu, mvals[outR]));
    }
  }
  float numL = 0, denL = 0, numR = 0, denR = 0;
  for (int s = 0; s < SAMPLES; s++) {
    float x = OUT_MIN + s * (OUT_MAX - OUT_MIN) / (SAMPLES - 1);
    numL += x * aggL[s]; denL += aggL[s];
    numR += x * aggR[s]; denR += aggR[s];
  }
  pwmL = denL ? int(numL / denL) : 0;
  pwmR = denR ? int(numR / denR) : 0;
}