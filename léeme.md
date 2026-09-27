\# Control PID de Balancín con MPU6050 y Sensor IR



Este proyecto implementa un lazo de control PID para equilibrar una bola en el centro de un riel o balancín de un solo eje, utilizando un Arduino UNO, un servo SG90, una IMU MPU6050 y un único sensor infrarrojo digital (MH-Sensor-Series) ubicado en la mitad de la barra.



\## Conexiones de Hardware



\### MPU6050 (IMU)

\* \*\*VCC\*\* -> 5V de Arduino

\* \*\*GND\*\* -> GND de Arduino

\* \*\*SCL\*\* -> Pin Analógico A5

\* \*\*SDA\*\* -> Pin Analógico A4



\### Servo SG90

\* \*\*Marrón\*\* (GND) -> GND de Arduino

\* \*\*Rojo\*\* (VCC) -> 5V de Arduino \*(Nota: Si zapatea, usar fuente externa de 5V compartiendo GND)\*

\* \*\*Naranja\*\* (Señal) -> Pin Digital 9



\### Sensor Infrarrojo (IR)

\* \*\*VCC\*\* -> 5V de Arduino

\* \*\*GND\*\* -> GND de Arduino

\* \*\*OUT / DO\*\* -> Pin Digital 2



\## Lógica del Código



1\. \*\*Lectura Avanzada (IMU):\*\* Combina los datos del acelerómetro y giroscopio usando un Filtro Complementario para calcular un ángulo combinado estable libre de vibraciones mecánicas.

2\. \*\*Detección por Zonas:\*\* El sensor IR actúa en lógica inversa (se apaga cuando la bola pasa por encima). Al activarse, revisa la inclinación actual en la IMU para saber hacia qué lado viaja la bola.

3\. \*\*Control PID Suave:\*\* Cambia el `setpointAngulo` a ± 7 grados en dirección opuesta para aplicar un contragolpe preciso y equilibrar la bola en el centro del riel sin movimientos bruscos.



