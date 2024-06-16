#pragma once
#include <Arduino.h>

enum class CardDir { Up, Right, Down, Left };
enum class RelDir { Front, Right, Back, Left };
enum class TileType { Normal, Victim, Black, Blue };

// -- Motors --
// Speed of the motor (0 = off, 255 = full speed)
#define MOTOR_SPEED 100
#define TURN_SPEED 100
#define TURN_SPEED_QUICK 200

// -- Ultrasonic sensor --
// When a distance is a wall (in cm)
#define US_THRESHOLD 15
// When the robot should stop moving and complete the advance, even if the full
// 30cm weren't driven
#define US_TOO_CLOSE 5
// Size of history of us
#define US_HISTORY_SIZE 5
// how much the amplitude of the us history is allowed to be
#define US_MAX_AMPLITUDE 5
// how long to drive backwards if the amplitude was too big
#define US_MAX_AMPLITUDE_TIME 500
// how many times to mesure us (must be odd)
#define US_POLL_COUNT 5
// Maximum amount that the uss can measure correctly (in cm)
#define US_MAX_DISTANCE 60

// -- Delays --
// how long to wait between checking orientation when turning (in ms)
#define TURN_DELAY 40
// how long to wait between checking distance when advancing (in ms)
#define ADVANCE_DELAY 100
// time an action takes (turning, advancing) when testing with virtual maze (in
// ms)
#define VIRT_DELAY 2000
// how amny ms to drive forward when bumping into wall
#define DRIVE_AGAINST_DELAY 2000
// how many ms to drive back when bumping into wall
#define DRIVE_AWAY_DELAY 400

// -- Debug messages --
// if we should print how much the robot has turned every time it checks
#define PRINT_TURNING_PROGRESS true
// if we should print how much the robot has driven every time it checks
#define PRINT_ADVANCE_PROGRESS true
// If a virtual maze is being simulated for testing
#define TESTING_VIRTUAL false
// If we should print the rgb values of every color measurement
#define PRINT_RGB_VALUES false

// -- Other --
// tile width (in cm)
#define TILE_SIZE 30
// minimum average inclination that a ramp needs to be recognized as one (in
// degrees)
#define RAMP_INCLINATION 17
// How manz degrees the robot is allowed to be off
#define TURN_TOL 2
// When turning, how many degrees the robot is allowed to be off for the exact
// turning to activate
#define EXACT_TURNING_RANGE 10

#define START_X 15
#define START_Y 15

extern CardDir rd;
extern TileType currentTileType;
extern int robotX;
extern int robotY;
extern bool victimDiscovered;

CardDir toCard(RelDir relDir, CardDir orientation);
RelDir toRel(CardDir cardDir, CardDir orientation);

CardDir opposite(CardDir cardDir);
RelDir opposite(RelDir relDir);

// Direction String
String dstr(RelDir dir);
String dstr(CardDir dir);

float minimum(float a, float b, float c, float d, float e);

// Shift `x` and `y` in direction `dir`
void shift(int &x, int &y, CardDir dir);

double getDegrees(CardDir robotDir, RelDir relDir);

void shiftArray(int array[], int arrLen);
