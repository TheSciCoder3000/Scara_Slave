#include "Stepper_CHAN_DE_VILLA.h"
#include "Arduino.h"

void Stepper::begin(int dir_pin, int step_pin, int *shift_1, int *shift_2, int shift_type, int en_pin, int ms1_pin, int ms2_pin, int ms3_pin)
{
    pinMode(dir_pin, OUTPUT);
    pinMode(step_pin, OUTPUT);

    STEP_PIN = step_pin;
    DIR_PIN = dir_pin;
    EN_INDEX = en_pin;
    MS1_INDEX = ms1_pin;
    MS2_INDEX = ms2_pin;
    MS3_INDEX = ms3_pin;

    SHIFT_1 = shift_1;
    SHIFT_2 = shift_2;
    SHIFT_TYPE = shift_type;
}

void Stepper::setDelay(int delay)
{
    DELAY_NUM = delay;
}

void Stepper::step()
{
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(DELAY_NUM);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(DELAY_NUM);
}

void Stepper::stepHigh()
{
    digitalWrite(STEP_PIN, 1);
}

void Stepper::stepLow()
{
    digitalWrite(STEP_PIN, 0);
}

void Stepper::setDir(int direction)
{
    digitalWrite(DIR_PIN, direction);
}

void delayMicros(unsigned long delay_us)
{
    if (delay_us)
    {

        unsigned long start_us = micros();
        if (delay_us > 50)
        {
            yield();
        }
        // See https://www.gammon.com.au/millis
        while (micros() - start_us < delay_us)
            ;
    }
}

void Stepper::setReso(int res1, int res2, int res3)
{
    writeData(MS1_INDEX, res1);
    writeData(MS2_INDEX, res2);
    writeData(MS3_INDEX, res3);
    sendData();
}

void Stepper::enableMotor()
{
    writeData(EN_INDEX, 0);
    sendData();
}

void Stepper::disableMotor()
{
    writeData(EN_INDEX, 1);
    sendData();
}

bool Stepper::isEnabled()
{
    if (SHIFT_TYPE == 1)
    {
        return bitRead(*SHIFT_1, EN_INDEX) != 1;
    }

    return bitRead(*SHIFT_2, EN_INDEX) != 1;
}

void Stepper::sendData()
{
    digitalWrite(LAT, LOW);
    shiftOut(DAT, CLK, LSBFIRST, *SHIFT_2);
    shiftOut(DAT, CLK, LSBFIRST, *SHIFT_1);
    digitalWrite(LAT, HIGH);
}

void Stepper::writeData(int index, int state)
{
    if (SHIFT_TYPE == 1)
    {
        bitWrite(*SHIFT_1, index, state);
    }
    else if (SHIFT_TYPE == 2)
    {
        bitWrite(*SHIFT_2, index, state);
    }
}