#ifndef STEPPER_H
#define STEPPER_H

class Stepper
{
public:
    int STEP_PIN;
    int DIR_PIN;
    int DELAY_NUM;
    int EN_INDEX;
    int MS1_INDEX;
    int MS2_INDEX;
    int MS3_INDEX;

    int DAT = 32;
    int LAT = 33;
    int CLK = 25;

    int *SHIFT_1;
    int *SHIFT_2;
    int SHIFT_TYPE;
    int NUM_STEPS;
    int DIRECTION;
    int PWM_CHANNEL;

    void begin(
        int dir_pin,
        int step_pin,
        int *shift_1,
        int *shift_2,
        int shift_type,
        int en_pin,
        int ms1_pin,
        int ms2_pin,
        int ms3_pin,
        int pwm_channel);

    void setDir(int direction);
    void setDelay(int delay);
    void setReso(int res1, int res2, int res3);

    void step();

    void enableMotor();
    void disableMotor();
    bool isEnabled();

    void stepHigh();
    void stepLow();

    void setupPwm(int frequency);
    void pwmStep();
    void setFreq(int delay);

private:
    void sendData();
    void writeData(int index, int state);
};

void delayMicros(unsigned long delay_us);

#endif
