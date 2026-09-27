#include <Wire.h>
#include <Servo.h>

const int MPU = 0x68; 
Servo miServo;

int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;

unsigned long tiempoPrevio;
float dt;

float anguloFiltradoX = 0.0; 
float anguloFiltradoY = 0.0; 

const float ANGULO_OFFSET_X = 7.32; 
const float ANGULO_OFFSET_Y = -8.18;   

const float GYRO_BIAS_X = 0.0; 
const float GYRO_BIAS_Y = 0.0; 

const int PIN_SERVO = 9;
const int PIN_IR = 2;
const int SERVO_CENTRO = 90;

// Ajuste según su estruvtura lololol
float Kp = 1.2;  
float Ki = 0.01; 
float Kd = 0.25;  

float error, errorPrevio, integral, derivativa, salidaPID;
float setpointAngulo = 0.0; 

void setup() {
  Serial.begin(115200); 
  Wire.begin();
  
  pinMode(PIN_IR, INPUT);
  
  miServo.attach(PIN_SERVO);
  miServo.write(SERVO_CENTRO);

  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1B); 
  Wire.write(0x00); 
  Wire.endTransmission(true);

  tiempoPrevio = micros();
}

void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); 

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); 
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  unsigned long tiempoActual = micros();
  dt = (tiempoActual - tiempoPrevio) / 1000000.0;
  tiempoPrevio = tiempoActual;

  float anguloAcX = atan2(AcY, sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180.0 / M_PI;
  float anguloAcY = atan2(-AcX, sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180.0 / M_PI;

  float velGyroX = (GyX - GYRO_BIAS_X) / 131.0;
  float velGyroY = (GyY - GYRO_BIAS_Y) / 131.0;

  anguloFiltradoX = 0.98 * (anguloFiltradoX + velGyroX * dt) + 0.02 * anguloAcX;
  anguloFiltradoY = 0.98 * (anguloFiltradoY + velGyroY * dt) + 0.02 * anguloAcY;

  float anguloFinalX = anguloFiltradoX + ANGULO_OFFSET_X;
  float anguloFinalY = anguloFiltradoY + ANGULO_OFFSET_Y;

  float magnitudInclinacion = sqrt(pow(anguloFinalX, 2) + pow(anguloFinalY, 2));
  if (anguloFinalX < 0) {
    magnitudInclinacion = -magnitudInclinacion;
  }

  int estadoIR = digitalRead(PIN_IR);

  // AJUSTADO EL ANGULO OBJETIVO A 7 GRADOS PARA CLAVAR EL FRENO
  if (estadoIR == LOW) {
    if (magnitudInclinacion > 0.0) {
      setpointAngulo = -7.0; 
    } else {
      setpointAngulo = 7.0;  
    }
  } else {
    setpointAngulo = 0.0; 
  }

  error = setpointAngulo - magnitudInclinacion;
  integral += error * dt;
  integral = constrain(integral, -20, 20);
  derivativa = (error - errorPrevio) / dt;
  errorPrevio = error;

  salidaPID = (Kp * error) + (Ki * integral) + (Kd * derivativa);

  int anguloServo = SERVO_CENTRO + (int)salidaPID;
  anguloServo = constrain(anguloServo, 60, 120); 

  miServo.write(anguloServo);

  Serial.print("IR:"); Serial.print(estadoIR);
  Serial.print(",Angulo:"); Serial.print(magnitudInclinacion);
  Serial.print(",Setpoint:"); Serial.print(setpointAngulo);
  Serial.print(",Servo:"); Serial.println(anguloServo);

  delay(15); 
}
