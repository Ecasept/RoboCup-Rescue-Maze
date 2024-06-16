#pragma once

class KA03Motor {
  public:
    KA03Motor(int dir, int pwm);
    void drehen(int speed, bool direction);

  private:
    int DIR;
    int PWM;
};
