#include "Stepper.h"
#include "Arduino.h"

void Stepper::begin(int dir_pin, int step_pin)
{
    pinMode(dir_pin, OUTPUT);
    pinMode(step_pin, OUTPUT);

    STEP_PIN = step_pin;
    DIR_PIN = dir_pin;
}

void Stepper::step()
{
    digitalWrite(STEP_PIN, HIGH);
    delayMicros(700);
    digitalWrite(STEP_PIN, LOW);
    delayMicros(700);
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
