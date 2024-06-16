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
extern ULTRASCHALLSENSOR usr; // right ultrasonic sensor
extern ULTRASCHALLSENSOR usl; // left ultrasonic sensor

extern Adafruit_TCS34725 colorsensor; // color sensor

extern KA03Motor motorBackRight;  // back right motor
extern KA03Motor motorBackLeft;   // back left motor
extern KA03Motor motorFrontRight; // front right motor
extern KA03Motor motorFrontLeft;  // front left motor

extern Adafruit_BNO055 bno; // Orientation sensor

extern Servo ejectServo; // Servo for ejecting rescue kits
#define VICTIM_PIN 12    // PIN to signal victim recognition

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
void turn(bool right, int speed);

} // namespace motor

// Functions directly accessing the sensors
namespace hardware {

TileType getFloorTileType();
void ejectRescuePacket();
int getUltrasonicMedian(ULTRASCHALLSENSOR *us);

} // namespace hardware

namespace util {
// euclidian distance between two three dimensional points
float euclideanDistance(float r1, float g1, float b1, int r2, int g2, int b2);
// get correct direction to turn when turning from angle a to angle b
// true = right, false = left
bool getTurnDir(int a, int b);
bool isNotMoving(int history[]);
} // namespace util

struct CorrectOrientationResult {
    bool wasWrong;
    sensors_event_t event;
};

// turn to goalOr (in degrees) and return last measured orientation
// NOTE: Does not turn off motor
CorrectOrientationResult correctOrientation(int targetOrientation);

// Turns the robot in direction `relDir`
void turn(RelDir relDir);

// unstuck the robot (move it back)
void unstuck();

bool advance();
