#include <Arduino.h>

//struct Motor {
//    int pin1, pin2, pin3, pin4;
//    int stepCount; // Controlar la posición actual en la secuencia de medio paso
//    int stepDelay; // Intervalo de tiempo entre pasos para controlar la velocidad
//
//    // Constructor que inicializa un motor con sus pines, el contador de pasos y el intervalo de tiempo entre pasos
//    Motor(int p1, int p2, int p3, int p4) : pin1(p1), pin2(p2), pin3(p3), pin4(p4), stepCount(0) {}
//};

struct Motor {
    int pin1, pin2, pin3, pin4;
    unsigned long stepInterval; // Intervalo entre pasos en microsegundos
    int stepsRemaining; // Pasos restantes
    bool direction; // true para adelante, false para atrás
    unsigned long lastStepTime; // Último momento en que se dio un paso
    int stepCount; // Controlar la posición actual en la secuencia de medio paso
};

//Motor motor1, motor2, motor3, motor4;

bool asignado = false;

// Definir los pines de los motores
Motor motor1 = { 8, 9, 10, 11};
Motor motor2 = { 4, 5, 6, 7};
Motor motor3 = { 24,26,28,30};
Motor motor4 = { 40,42,44, 46};

Motor* motors[4] = { &motor1, &motor2, &motor3, &motor4 };

const int motorSteps[8][4] = {
  {HIGH, LOW, LOW, LOW}, {HIGH, HIGH, LOW, LOW}, {LOW, HIGH, LOW, LOW}, {LOW, HIGH, HIGH, LOW},
  {LOW, LOW, HIGH, LOW}, {LOW, LOW, HIGH, HIGH}, {LOW, LOW, LOW, HIGH}, {HIGH, LOW, LOW, HIGH}
};

void setupMotor(Motor& motor) {
    pinMode(motor.pin1, OUTPUT);
    pinMode(motor.pin2, OUTPUT);
    pinMode(motor.pin3, OUTPUT);
    pinMode(motor.pin4, OUTPUT);
}

void stepMotor(Motor& motor, bool direction) {
    if (direction) { // CW - Sentido horario
        motor.stepCount = (motor.stepCount + 1) % 8; }
    else { // CCW - Sentido antihorario
        motor.stepCount = (motor.stepCount + 7) % 8; }

    digitalWrite(motor.pin1, motorSteps[motor.stepCount][0]);
    digitalWrite(motor.pin2, motorSteps[motor.stepCount][1]);
    digitalWrite(motor.pin3, motorSteps[motor.stepCount][2]);
    digitalWrite(motor.pin4, motorSteps[motor.stepCount][3]);
}

void stopMotor(Motor& motor) {
    digitalWrite(motor.pin1, LOW);
    digitalWrite(motor.pin2, LOW);
    digitalWrite(motor.pin3, LOW);
    digitalWrite(motor.pin4, LOW);
}

void moveMotorsIfNeeded() {
    unsigned long currentTime = micros();
    for (int i = 0; i < 4; i++) {
        if (motors[i]->stepsRemaining > 0) {
            if (currentTime - motors[i]->lastStepTime >= motors[i]->stepInterval) {
                stepMotor(*motors[i], motors[i]->direction);
                motors[i]->stepsRemaining = motors[i]->stepsRemaining - 1;
                motors[i]->lastStepTime = currentTime;
                if (motors[i]->stepsRemaining <= 0) {
                    stopMotor(*motors[i]); 
                    Serial.print("Motor ");
                    Serial.print(i);
                    Serial.println(" detenido.");
                }
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
    motor1.stepCount = 0;
    motor2.stepCount = 0;
    motor3.stepCount = 0;
    motor4.stepCount = 0;

    setupMotor(motor1);
    setupMotor(motor2);
    setupMotor(motor3);
    setupMotor(motor4);
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Lee el comando hasta que encuentre un salto de línea
    }
    if (!asignado)
    {   
    motor1.stepsRemaining=1580; // Mueve el motor 1 512 pasos en sentido antihorario
    motor1.direction = true;
    motor1.stepInterval = 1100; // 1600 microsegundos entre pasos

    motor2.stepsRemaining = 20; // Mueve el motor 2 512 pasos en sentido horario
    motor2.direction = true;
    motor2.stepInterval = 1100; // 1600 microsegundos entre pasos

    motor3.stepsRemaining = 1580; // Mueve el motor 3 512 pasos en sentido antihorario
    motor3.direction = false;
    motor3.stepInterval = 1100; // 1600 microsegundos entre pasos

    motor4.stepsRemaining = 40; // Mueve el motor 4 512 pasos en sentido horario
    motor4.direction = true;
    motor4.stepInterval = 1100; // 1600 microsegundos entre pasos

    asignado = true;
	}

    moveMotorsIfNeeded();
}
