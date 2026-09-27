#include <Wire.h>

const int MPU = 0x68; 

int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;

unsigned long tiempoPrevio;
float dt;
float anguloFiltradoX = 0.0; // Ángulo final en el eje X


// CONFIGURACIÓN DE CALIBRACIÓN MANUAL
const float ANGULO_OFFSET_X = 0.0; 

// Compensación para el giroscopio en X
const float GYRO_BIAS_X = 0.0; 

void setup() {
  Serial.begin(115200); 
  Wire.begin();
  
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  // Configurar escala del Giroscopio a +/- 250 grados/seg
  Wire.beginTransmission(MPU);
  Wire.write(0x1B); 
  Wire.write(0x00); 
  Wire.endTransmission(true);

  tiempoPrevio = micros();
}

void loop() {
  // Leer los registros de Acelerómetro y Giroscopio
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); 

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // Ignorar temperatura
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Calcular diferencial de tiempo (dt)
  unsigned long tiempoActual = micros();
  dt = (tiempoActual - tiempoPrevio) / 1000000.0;
  tiempoPrevio = tiempoActual;

  // Calcular el ángulo de inclinación en X usando el acelerómetro
  float anguloAcX = atan2(AcY, AcZ) * 180.0 / M_PI;

  // Convertir la velocidad angular del Giroscopio en X a grados/segundo
  float velGyroX = (GyX - GYRO_BIAS_X) / 131.0;

  // Filtro Complementario enfocado estrictamente en el Eje X
  anguloFiltradoX = 0.98 * (anguloFiltradoX + velGyroX * dt) + 0.02 * anguloAcX;

  // Aplicar Offset
  float anguloFinalX = anguloFiltradoX + ANGULO_OFFSET_X;

  Serial.print("Angulo_X_Balancin:");
  Serial.println(anguloFinalX);

  delay(5);
}

