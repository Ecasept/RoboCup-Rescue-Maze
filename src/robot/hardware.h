#pragma once
#include "common.h"
#include "maze.h"
#include <Adafruit_BNO055.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include <ka03_driver.h>
#include <ultraschall.h>
// #include <vl53l4cx_class.h>

extern ULTRASCHALLSENSOR usf; // front ultrasonic sensor

/*#define DEV_I2C Wire
#define TOF_XSHOT 5

extern VL53L4CX tof;*/

extern ULTRASCHALLSENSOR usr;         // right ultrasonic sensor
extern ULTRASCHALLSENSOR usl;         // left ultrasonic sensor
extern Adafruit_TCS34725 colorsensor; // color sensor

extern KA03Motor motorrechtshinten; // back right motor
extern KA03Motor motorlinkshinten;  // back left motor
extern KA03Motor motorrechtsvorne;  // front right motor
extern KA03Motor motorlinksvorne;   // front left motor

extern Adafruit_BNO055 bno; // Orientation sensor

extern Servo ejectServo;

#define VICTIM_PIN 12

namespace motor {
// Turns the motors of all of the wheels on with strength `strength`. If
// `forwards` is `true`, then the robot drives forwards, otherwise backwards.
void on(int strength, bool forwards);

void ramp();

void on();

// Turns the motors of all of the wheels off.
void off();

// Turns on the motors so that the robot turns right/left, depending on the
// parameter
void turn(bool right);

void turnQuick(bool right);
} // namespace motor

namespace hardware {

struct CorrectOrientationResult {
    bool wasWrong;
    sensors_event_t event;
};

// void setup_hardware();

int getUltrasonicMedian(ULTRASCHALLSENSOR *us);

float euclideanDistance(float r1, float g1, float b1, int r2, int g2, int b2);

TileType getFloorTileType();

void ejectRescuePacket();

// turn to goalOr (in degrees) and return last measured orientation
// NOTE: Does not turn off motor
CorrectOrientationResult correctOrientation(int targetOrientation, bool quick);

// Returns the direction in degrees that the robot should be facing after
// turning in the direction `relDir`, when facing `robotDir`.
double getGoalOrientation(CardDir robotDir, RelDir relDir);

// Turns the robot in direction `relDir`
void turn(RelDir relDir);

bool isNotMoving(int history[]);

// unstuck the robot (move it back)
void unstuck();

bool advance();
} // namespace hardware
