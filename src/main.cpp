#include "Wire.h"
#include "Arduino.h"
#include "A4988.h"
#include "Stepper.h"

#define STP1 25
#define DIR1 33
#define STP2 4
#define DIR2 16
#define STP3 17
#define DIR3 5
#define STP4 18
#define DIR4 19

#define MS1 14
#define MS2 27
#define MS3 26

#define SERVO 13

#define SLAVE_ADD 69

#define CC 0
#define CL 1

Stepper stepper1;
Stepper stepper2;
Stepper stepper3;
Stepper stepper4;

String received;
void receiveEvent(int);
void move_stepper();
int x;

int motor1_steps = 0;
int motor2_steps = 0;
int motor3_steps = 0;
int motor4_steps = 0;

int motor1_direction = 0;
int motor2_direction = 0;
int motor3_direction = 0;
int motor4_direction = 0;

void setup()
{
  Wire.begin(SLAVE_ADD);

  pinMode(SERVO, OUTPUT);

  stepper1.begin(DIR1, STP1);
  stepper2.begin(DIR2, STP2);
  stepper3.begin(DIR3, STP3);
  stepper4.begin(DIR4, STP4);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  // Wire.setClock(400000);
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
}

void loop()
{
  unsigned long start_us = micros();

  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');

    if (command == "who u?")
    {
      Serial.println("Ako si: Slave");
    }
  }

  move_stepper();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  received = "";

  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    received += c;
  }

  // return if command is none
  if (received == "")
    return;

  String outputType = received.substring(0, 4); // select the first 4 characters of command

  if (outputType == "SERV")
  {
    int servoDegree = received.substring(4, received.length()).toInt();
  }
  else if (outputType == "STEP")
  {
    int motor_num = received.substring(4, 5).toInt();
    int motor_steps = received.substring(7, received.length()).toInt();
    int motor_direction = (received.substring(5, 7) == "CC" ? 0 : 1);

    switch (motor_num)
    {
    case 1:
      motor1_steps = motor_steps;
      motor1_direction = motor_direction;
      break;
    case 2:
      motor2_steps = motor_steps;
      motor2_direction = motor_direction;
      break;
    case 3:
      motor3_steps = motor_steps;
      motor3_direction = motor_direction;
      break;
    case 4:
      motor4_steps = motor_steps;
      motor4_direction = motor_direction;
      break;

    default:
      Serial.println("Invalid STEP Command!");
      break;
    }

    Serial.println("Motor: " + String(motor_num) + " \t | Steps: " + String(motor_steps) + " \t | direction: " + String((motor_direction == 0 ? "Counter Clockwise" : "Clockwise")));
  }
  else if (outputType == "RESO")
  {
    int ms1 = received.substring(4, 5).toInt();
    int ms2 = received.substring(5, 6).toInt();
    int ms3 = received.substring(6, 7).toInt();

    digitalWrite(MS1, ms1);
    digitalWrite(MS2, ms2);
    digitalWrite(MS3, ms3);
  }
}

// ============================== STEPPER FUNCTIONS ==============================

void move_stepper()
{
  if (motor1_steps > 0)
  {
    stepper1.setDir(motor1_direction);
    stepper1.step();
    motor1_steps--;
  }

  if (motor2_steps > 0)
  {
    stepper2.setDir(motor2_direction);
    stepper2.step();
    motor2_steps--;
  }

  if (motor3_steps > 0)
  {
    stepper3.setDir(motor3_direction);
    stepper3.step();
    motor3_steps--;
  }

  if (motor4_steps > 0)
  {
    stepper4.setDir(motor4_direction);
    stepper4.step();
    motor4_steps--;
  }
}

// GRIPPER - CC INVERTED
// IDEAL FORTESTING 300 stps
