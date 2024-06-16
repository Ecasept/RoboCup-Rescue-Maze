#include "error.h"
#include <Arduino.h>
void raise(const char *message) {
    Serial.print("Error: ");
    Serial.println(message);
    delay(1000);
    exit(1);
}
