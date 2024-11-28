#include "Wire.h"
#include "Arduino.h"
#include "Stepper_CHAN_DE_VILLA.h"
#include "ESP32Servo.h"

#define CLK 25
#define DAT 32
#define LAT 33

#define STP1 27
#define DIR1 14
#define STP2 4
#define DIR2 16
#define STP3 19
#define DIR3 18
#define STP4 17
#define DIR4 5

#define SERVO 13

#define SLAVE_ADD 69

#define CC 0
#define CL 1

Stepper stepper1;
Stepper stepper2;
Stepper stepper3;
Stepper stepper4;

Servo gripper;

void receiveEvent(int);
void move_stepper();
void set_stepper_reso(int, int, int, int);
int x;

int motor1_steps = 0;
int motor2_steps = 0;
int motor3_steps = 0;
int motor4_steps = 0;

int motor1_direction = 0;
int motor2_direction = 0;
int motor3_direction = 0;
int motor4_direction = 0;

int shift1 = 0;
int shift2 = 0;
int DELAY_MICROS = 500;

int SHIFT_1 = 0;
int SHIFT_2 = 0;
int *SHIFT_1_ptr = &SHIFT_1;
int *SHIFT_2_ptr = &SHIFT_2;

void setup()
{
  Wire.begin(SLAVE_ADD);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  gripper.setPeriodHertz(50); // standard 50 hz servo
  gripper.attach(SERVO, 1000, 2000);

  pinMode(CLK, OUTPUT);
  pinMode(DAT, OUTPUT);
  pinMode(LAT, OUTPUT);

  stepper1.begin(DIR1, STP1, SHIFT_1_ptr, SHIFT_2_ptr, 1, 7, 0, 1, 2);
  stepper2.begin(DIR2, STP2, SHIFT_1_ptr, SHIFT_2_ptr, 1, 3, 4, 5, 6);
  stepper3.begin(DIR3, STP3, SHIFT_1_ptr, SHIFT_2_ptr, 2, 7, 0, 1, 2);
  stepper4.begin(DIR4, STP4, SHIFT_1_ptr, SHIFT_2_ptr, 2, 3, 4, 5, 6);

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
    else if (command == "show enabled")
    {
      Serial.print("STEP1: " + String(stepper1.isEnabled()));
      Serial.print("\t|\tSTEP2: " + String(stepper2.isEnabled()));
      Serial.print("\t|\tSTEP3: " + String(stepper3.isEnabled()));
      Serial.print("\t|\tSTEP4: " + String(stepper4.isEnabled()));
      Serial.println("");
    }
    else
    {
      String commandKey = command.substring(0, 5);
      if (commandKey == "SHIFT")
      {
        int shiftNum = command.substring(5, 6).toInt();

        if (shiftNum == 1)
        {
          int index = command.substring(6, 7).toInt();
          int value = command.substring(7, 8).toInt();
          bitWrite(shift1, index, value);
        }
        else if (shiftNum == 2)
        {
          int index = command.substring(6, 7).toInt();
          int value = command.substring(7, 8).toInt();
          bitWrite(shift2, index, value);
        }
        else if (shiftNum == 0)
        {
          shift1 = 0;
          shift2 = 0;
        }

        digitalWrite(LAT, LOW);
        shiftOut(DAT, CLK, LSBFIRST, shift2);
        shiftOut(DAT, CLK, LSBFIRST, shift1);
        digitalWrite(LAT, HIGH);
      }
    }
  }

  move_stepper();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  String received = "";

  while (Wire.available() > 1) // loop through all but the last
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
    Serial.println("SERVO: " + String(servoDegree));
    gripper.write(servoDegree);
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

    // Serial.println("Motor: " + String(motor_num) + " \t | Steps: " + String(motor_steps) + " \t | direction: " + String((motor_direction == 0 ? "Counter Clockwise" : "Clockwise")));
  }
  else if (outputType == "RESO")
  {
    int motor_index = received.substring(4, 5).toInt();
    int ms1 = received.substring(5, 6).toInt();
    int ms2 = received.substring(6, 7).toInt();
    int ms3 = received.substring(7, 8).toInt();

    Serial.print("Module No: " + String(motor_index));
    Serial.print("\t|\tMS1: " + String(ms1));
    Serial.print("\t|\tMS2: " + String(ms2));
    Serial.print("\t|\tMS3: " + String(ms3));
    Serial.println("");

    set_stepper_reso(motor_index, ms1, ms2, ms3);
  }
  else if (outputType == "ENAB")
  {
    int enable_num = received.substring(4, 5).toInt();
    Serial.println("Enable num: " + String(enable_num));

    switch (enable_num)
    {
    case 0:
      stepper1.disableMotor();
      stepper2.disableMotor();
      stepper3.disableMotor();
      stepper4.disableMotor();
      Serial.println("disabling all motor");
      break;
    case 1:
      stepper1.enableMotor();
      Serial.println("Enable Motor 1");
      break;
    case 2:
      stepper2.enableMotor();
      Serial.println("Enable Motor 2");
      break;
    case 3:
      stepper3.enableMotor();
      Serial.println("Enable Motor 3");
      break;
    case 4:
      stepper4.enableMotor();
      Serial.println("Enable Motor 4");
      break;
    case 5:
      stepper1.enableMotor();
      stepper2.enableMotor();
      stepper3.enableMotor();
      stepper4.enableMotor();
      Serial.println("Enable All Motor");
      break;
    default:
      Serial.println("Invalid ENABLE num: " + String(enable_num));
      break;
    }
  }
  else if (outputType == "DELA")
  {
    int delay = received.substring(4, received.length()).toInt();
    DELAY_MICROS = delay;

    stepper1.setDelay(delay);
    stepper2.setDelay(delay);
    stepper3.setDelay(delay);
    stepper4.setDelay(delay);
    Serial.println("Setting Delay: " + String(DELAY_MICROS));
  }
  else if (outputType == "STOP")
  {
    int motor = received.substring(4, 5).toInt();

    switch (motor)
    {
    case 1:
      motor1_steps = 0;
      break;
    case 2:
      motor2_steps = 0;
      break;
    case 3:
      motor3_steps = 0;
      break;
    case 4:
      motor4_steps = 0;
      break;

    default:
      Serial.println("Invalid STOP Command!");
      break;
    }
  }
}

// ============================== STEPPER FUNCTIONS ==============================

void set_stepper_reso(int stepper_num, int res1, int res2, int res3)
{
  switch (stepper_num)
  {
  case 1:
    stepper1.setReso(res1, res2, res3);
    break;
  case 2:
    stepper2.setReso(res1, res2, res3);
    break;
  case 3:
    stepper3.setReso(res1, res2, res3);
    break;
  case 4:
    stepper4.setReso(res1, res2, res3);
    break;

  default:
    Serial.println("Invalid STOP Command!");
    break;
  }
}

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
