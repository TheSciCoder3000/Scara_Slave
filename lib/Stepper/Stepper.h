#ifndef STEPPER_H
#define STEPPER_H

class Stepper
{
public:
    int STEP_PIN;
    int DIR_PIN;
    int DELAY_NUM;
    void begin(int dir_pin, int step_pin);
    void setDir(int direction);
    void setDelay(int delay);
    void step();
};

void delayMicros(unsigned long delay_us);

#endif
