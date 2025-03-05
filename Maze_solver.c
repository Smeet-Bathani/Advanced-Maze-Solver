#include <Arduino.h>
#include <Servo.h>

// Define ultrasonic sensor pins
const int trigPinFront = 10;
const int echoPinFront = 11;
const int trigPinLeft = 12;
const int echoPinLeft = 13;
const int trigPinRight = 8;
const int echoPinRight = 9;

// Motor driver pins
const int motorLeft1 = 5;
const int motorLeft2 = 6;
const int motorRight1 = 3;
const int motorRight2 = 4;
const int enableLeft = 7;
const int enableRight = 2;

// Servo motor for ultrasonic sensor
Servo servoSensor;
const int servoPin = A0;

// Define distances
const int minDistance = 10; // Minimum distance to obstacle (cm)
const int maxDistance = 50; // Maximum distance to consider a path (cm)

// Movement parameters
const int baseSpeed = 150; // Base motor speed
const int turnSpeed = 180; // Speed for turning
const int turnDelay = 300; // Delay for 90-degree turn (ms)

// Function to measure distance using ultrasonic sensor
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // Calculate distance in cm
}

// Function to control motors
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  analogWrite(enableLeft, leftSpeed);
  analogWrite(enableRight, rightSpeed);

  if (leftSpeed > 0) {
    digitalWrite(motorLeft1, HIGH);
    digitalWrite(motorLeft2, LOW);
  } else {
    digitalWrite(motorLeft1, LOW);
    digitalWrite(motorLeft2, HIGH);
  }

  if (rightSpeed > 0) {
    digitalWrite(motorRight1, HIGH);
    digitalWrite(motorRight2, LOW);
  } else {
    digitalWrite(motorRight1, LOW);
    digitalWrite(motorRight2, HIGH);
  }
}

// Function to move forward
void moveForward() {
  setMotorSpeeds(baseSpeed, baseSpeed);
}

// Function to turn left
void turnLeft() {
  setMotorSpeeds(-turnSpeed, turnSpeed);
  delay(turnDelay);
  setMotorSpeeds(0, 0); // Stop after turning
}

// Function to turn right
void turnRight() {
  setMotorSpeeds(turnSpeed, -turnSpeed);
  delay(turnDelay);
  setMotorSpeeds(0, 0); // Stop after turning
}

// Function to scan surroundings with servo
void scanSurroundings(int &frontDistance, int &leftDistance, int &rightDistance) {
  servoSensor.write(90); // Center the servo
  delay(500);
  frontDistance = getDistance(trigPinFront, echoPinFront);
  servoSensor.write(0); // Turn servo left
  delay(500);
  leftDistance = getDistance(trigPinLeft, echoPinLeft);
  servoSensor.write(180); // Turn servo right
  delay(500);
  rightDistance = getDistance(trigPinRight, echoPinRight);
  servoSensor.write(90); // Return servo to center
  delay(500);
}

void setup() {
  // Initialize sensor pins
  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  // Initialize motor driver pins
  pinMode(motorLeft1, OUTPUT);
  pinMode(motorLeft2, OUTPUT);
  pinMode(motorRight1, OUTPUT);
  pinMode(motorRight2, OUTPUT);
  pinMode(enableLeft, OUTPUT);
  pinMode(enableRight, OUTPUT);

  // Initialize servo motor
  servoSensor.attach(servoPin);

  // Begin serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  int frontDistance, leftDistance, rightDistance;
  scanSurroundings(frontDistance, leftDistance, rightDistance);

  // Print distances for debugging
  Serial.print("Front: ");
  Serial.print(frontDistance);
  Serial.print(" cm, Left: ");
  Serial.print(leftDistance);
  Serial.print(" cm, Right: ");
  Serial.println(rightDistance);

  // Maze solving logic
  if (frontDistance > maxDistance) {
    moveForward();
  } else if (leftDistance > maxDistance) {
    turnLeft();
    moveForward();
  } else if (rightDistance > maxDistance) {
    turnRight();
    moveForward();
  } else {
    // Dead end: turn around
    turnRight();
    turnRight();
    moveForward();
  }

  delay(100); // Short delay for stability
}
