#include "ka03_driver.h"
#include "Arduino.h"

KA03Motor::KA03Motor(int dir, int pwm) {
    DIR = dir;
    PWM = pwm;

    pinMode(DIR, OUTPUT);
    pinMode(PWM, OUTPUT);
}

void KA03Motor::drehen(int speed, bool direction) {
    if (direction == true) {
        digitalWrite(DIR, HIGH);
    } else {
        digitalWrite(DIR, LOW);
    }

    analogWrite(PWM, speed);
}
