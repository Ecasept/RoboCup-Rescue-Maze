#ifndef ultraschall_h
#define ultraschall_h

class ULTRASCHALLSENSOR {
  public:
    ULTRASCHALLSENSOR(int trigger, int echo);
    long auslesen();

  private:
    int triggerpin;
    int echopin;
    long entfernung;
    long zeit;
};
#endif
