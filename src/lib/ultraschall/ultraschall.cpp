#include "ultraschall.h"
#include "Arduino.h"

ULTRASCHALLSENSOR::ULTRASCHALLSENSOR(int trigger, int echo) {
    triggerpin = trigger;
    echopin = echo;
    pinMode(triggerpin, OUTPUT);
    pinMode(echopin, INPUT);
}

long ULTRASCHALLSENSOR::auslesen() {
    digitalWrite(triggerpin, LOW);
    delayMicroseconds(3);
    digitalWrite(triggerpin, HIGH); // Trigger Impuls 10 us
    delayMicroseconds(10);
    digitalWrite(triggerpin, LOW);
    zeit = pulseIn(echopin, HIGH); // Echo-Zeit messen
    zeit = (zeit / 2);             // Zeit halbieren
    entfernung = zeit / 29.1;      // Zeit in Zentimeter umrechnen
    return (entfernung);
}
