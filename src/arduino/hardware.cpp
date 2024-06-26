#include "hardware.h"
#include "common.h"
#include "error.h"

// VL53L4CX tof(&DEV_I2C, TOF_XSHOT);

// trigger, echo
ULTRASCHALLSENSOR usf = ULTRASCHALLSENSOR(39, 27);
ULTRASCHALLSENSOR usr = ULTRASCHALLSENSOR(31, 25);
ULTRASCHALLSENSOR usl = ULTRASCHALLSENSOR(37, 35);
Adafruit_TCS34725 colorsensor =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

KA03Motor motorrechtshinten(7, 6);
KA03Motor motorlinkshinten(8, 9);
KA03Motor motorrechtsvorne(13, 11);
KA03Motor motorlinksvorne(4, 5);

Adafruit_BNO055 bno = Adafruit_BNO055(20);

Servo ejectServo;

// Color Averages
int redAvg[3] = {94, 32, 21};
float blackAvg[3] = {18, 12, 7};
float whiteAvg[3] = {193, 182, 107}; //{221, 206, 118};
float grayAvg[3] = {136, 105, 61};
float blueAvg[3] = {0, 0, 0};

namespace motor {

void on(int strength, bool forwards) {
    motorrechtshinten.drehen(strength, forwards);
    motorrechtsvorne.drehen(strength, forwards);
    motorlinkshinten.drehen(strength, forwards);
    motorlinksvorne.drehen(strength, forwards);
}

void ramp() { on(); }

void on() { on(MOTOR_SPEED, true); }

void off() {
    motorrechtshinten.drehen(0, true);
    motorrechtsvorne.drehen(0, true);
    motorlinkshinten.drehen(0, true);
    motorlinksvorne.drehen(0, true);
}

void turn(bool right, int speed) {
    motorrechtsvorne.drehen(speed, !right);
    motorrechtshinten.drehen(speed, !right);
    motorlinksvorne.drehen(speed, right);
    motorlinkshinten.drehen(speed, right);
}

} // namespace motor

namespace hardware {
int getUltrasonicMedian(ULTRASCHALLSENSOR *us) {
    int measurements[US_POLL_COUNT];
    for (int i = 0; i < US_POLL_COUNT; i++) {
        measurements[i] = us->auslesen();
    }

    int min = 10000;
    int max = 0;
    int minIndex = -1;
    int maxIndex = -1;
    int realIndex = -1;
    // NOTE: US_POLL_COUNT needs to be odd
    int rounds = US_POLL_COUNT;
    while (true) {
        for (int i = 0; i < US_POLL_COUNT; i++) {
            int val = measurements[i];
            if (val == -1) {
                continue;
            }

            if (val < min) {
                min = val;
                minIndex = i;
            } else if (val < max) {
                max = val;
                maxIndex = i;
            } else {
                realIndex = i;
            }
        }
        measurements[minIndex] = -1;
        measurements[maxIndex] = -1;
        rounds = rounds - 2;

        if (rounds == 2) {
            for (int i = 0; i < 100; i++) {
                Serial.println("odd US_POLL_COUNT");
                delay(100);
            }
        }
        if (rounds <= 2) {
            break;
        }
    }
    int measurement = measurements[realIndex];
    return measurement;
}
TileType getFloorTileType() {
    if (TESTING_VIRTUAL) {
        // Only happens when advancingh
        return TileType::Normal;
    } else {
        uint16_t r, g, b, c;
        // colorsensor.setInterrupt(false); // led
        colorsensor.getRawData(&r, &g, &b, &c);
        // colorsensor.setInterrupt(true);
        if (PRINT_RGB_VALUES) {
            Serial.print("   Colors: r=");
            Serial.print(r);
            Serial.print(" g=");
            Serial.print(g);
            Serial.print(" b=");
            Serial.print(b);
            Serial.print(" c=");
            Serial.println(c);
        }

        // measure distance between measured color and color averages and return
        // color with lowest distance
        float rDist =
            util::euclideanDistance(r, g, b, redAvg[0], redAvg[1], redAvg[2]);
        float bDist = util::euclideanDistance(r, g, b, blackAvg[0], blackAvg[1],
                                              blackAvg[2]);
        float wDist = util::euclideanDistance(r, g, b, whiteAvg[0], whiteAvg[1],
                                              whiteAvg[2]);
        float gDist = util::euclideanDistance(r, g, b, grayAvg[0], grayAvg[1],
                                              grayAvg[2]);
        float blueDist = util::euclideanDistance(r, g, b, blueAvg[0],
                                                 blueAvg[1], blueAvg[2]);

        if (PRINT_RGB_VALUES) {
            Serial.println(rDist);
            Serial.println(bDist);
            Serial.println(wDist);
            Serial.println(gDist);
        }
        float smallest = minimum(rDist, bDist, wDist, gDist, blueDist);
        if (smallest == rDist) {
            Serial.println("Detected Victim");
            return TileType::Victim;
        } else if (smallest == bDist) {
            Serial.println("Detected Black");
            return TileType::Black;
        } else if (smallest == blueDist) {
            Serial.println("Detected Blue");
            return TileType::Blue;
        } else {
            Serial.println("Detected Normal");
            return TileType::Normal;
        }
    }
}
void ejectRescuePacket() {
    Serial.print("Ejecting rescue kit...");

    digitalWrite(VICTIM_PIN, HIGH);

    ejectServo.write(32);
    delay(500);

    // throw out slowly (~2000ms)
    for (int angle = 32; angle <= 180; angle++) {
        ejectServo.write(angle);
        // Serial.println(angle);
        delay(10);
    }
    for (int i = 0; i < 5; i++) {
        digitalWrite(VICTIM_PIN, HIGH);
        delay(500);
        digitalWrite(VICTIM_PIN, LOW);
        delay(500);
    }
    Serial.println("done");
}
} // namespace hardware

namespace util {
float euclideanDistance(float r1, float g1, float b1, int r2, int g2, int b2) {
    return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}
bool isNotMoving(int history[]) {
    int minVal = 10000;
    int maxVal = 0;
    for (int i = 0; i < US_HISTORY_SIZE; i++) {
        int val = history[i];
        if (val < minVal) {
            minVal = val;
        }
        if (val > maxVal) {
            maxVal = val;
        }
    }
    Serial.print("History Amplitude: ");
    Serial.println(maxVal - minVal);
    return ((maxVal - minVal) < US_MAX_AMPLITUDE) and (minVal < 60);
}
bool getTurnDir(int a, int b) {
    int diff = b - a;
    if (diff < -180)
        return true;
    if (diff < 0)
        return false;
    if (diff < 180)
        return true;
    else {
        return false;
    }
}
} // namespace util

CorrectOrientationResult correctOrientation(int targetOrientation) {
    auto getDiff = [](int c, int g) {
        int diff1 = abs(c - g);
        int diff2 = 360 - diff1;
        int diff = diff1 < diff2 ? diff1 : diff2; // min
        return diff;
    };

    CorrectOrientationResult res;
    res.wasWrong = false;

    sensors_event_t event;
    int currentOrientation;

    while (true) {
        bno.getEvent(&event);
        currentOrientation = event.orientation.x;

        auto diff = getDiff(currentOrientation, targetOrientation);

        if (diff <= TURN_TOL)
            break;

        if (PRINT_TURNING_PROGRESS) {
            Serial.print("Correcting orientation - currently at: ");
            Serial.println(currentOrientation);
        }

        bool dir = util::getTurnDir(currentOrientation, targetOrientation);

        Serial.print(dir);

        if (diff <= EXACT_TURNING_RANGE) {
            motor::turn(dir, TURN_SPEED);
            delay(TURN_DELAY);
            motor::off();
            delay(TURN_DELAY);
        } else {

            motor::turn(dir, TURN_SPEED_QUICK);
        }
    }

    res.event = event;
    return res;
}

void turn(RelDir relDir) {
    sensors_event_t event;

    bno.getEvent(&event);
    double startingOrientation = event.orientation.x;
    double goalOrientation = getDegrees(rd, relDir);

    if (relDir == RelDir::Right) {
        // Turn internal robot direction
        rd = static_cast<CardDir>((static_cast<int>(rd) + 1) % 4);
    } else if (relDir == RelDir::Left) {
        // Turn internal robot direction
        rd = static_cast<CardDir>((static_cast<int>(rd) - 1 + 4) % 4);
    } else if (relDir == RelDir::Front) {
        // Don't need to turn
        return;
    } else {
        raise("Unknown direction to turn towards");
    }

    Serial.print("Turning from ");
    Serial.print(startingOrientation);
    Serial.print(" to ");
    Serial.println(goalOrientation);

    correctOrientation(goalOrientation);

    motor::off();
}

void unstuck() {
    Serial.print("Moving back...");
    motor::on(MOTOR_SPEED, false);
    delay(US_MAX_AMPLITUDE_TIME);
    Serial.println("done");
}

void advanceRamp() {
    int vertInclination = 0;

    motor::ramp();
    while (true) {

        // fix orientation and get orientation
        auto res = correctOrientation(getDegrees(rd, RelDir::Front));
        // measure inclination
        double vertInclination = abs(res.event.orientation.y);

        // check if on ramp
        if (vertInclination < RAMP_INCLINATION) {
            return;
        }
    }
}

bool advance() {
    // turnToDidTurning = false;
    // turnToCalls = 0;

    int history[US_HISTORY_SIZE] = {10, 30, 50, 70, 90};

    bool victimDiscoveredLastTile = victimDiscovered;
    victimDiscovered = false;
    bool black_found = false;

    int startingDistance = /*getUltrasonicMedian(&usf);*/ usf.auslesen();
    int goalDistance = startingDistance - TILE_SIZE;

    int vertInclination = 0;

    Serial.print("Advancing (distance: ");
    Serial.print(startingDistance);
    Serial.println(")...");

    motor::on();
    if (TESTING_VIRTUAL) {
        delay(VIRT_DELAY);
    }

    while (true and !TESTING_VIRTUAL) {

        // fix orientation and get orientation
        auto res = correctOrientation(getDegrees(rd, RelDir::Front));
        // measure inclination
        double vertInclination = abs(res.event.orientation.y);

        // check if on ramp
        if (vertInclination >= RAMP_INCLINATION) {
            advanceRamp();
            return true;
        }

        // check tile color
        TileType tt = hardware::getFloorTileType();
        if (tt == TileType::Black) {
            Serial.println("Black tile found!");
            // drive back to start
            goalDistance = startingDistance;
            black_found = true;
        } else if (tt == TileType::Victim) {
            if (!victimDiscoveredLastTile) {
                victimDiscovered = true;
                Serial.println("Victim found and discovered!");
            }
        }

        int currentDistance =
            usf.auslesen(); // getUltrasonicMedian(&usf);//usf.auslesen();
        int distanceLeft = currentDistance - goalDistance;
        distanceLeft = distanceLeft % TILE_SIZE;

        shiftArray(history, US_HISTORY_SIZE);
        history[US_HISTORY_SIZE - 1] = currentDistance;
        if (util::isNotMoving(history) and !black_found) {
            unstuck();
            Serial.println("Did not move enough - moving backwards");
            history[0] = 10;
            history[1] = 30;
            history[2] = 50;
            history[3] = 70;
            history[4] = 90;
        }

        // update direction (forward/backward)
        motor::on(MOTOR_SPEED, distanceLeft > 0);

        if (PRINT_ADVANCE_PROGRESS) {
            int dif = startingDistance - currentDistance;
            Serial.print("distance: ");
            Serial.print(currentDistance);
            Serial.print(" - distance left: ");
            Serial.print(distanceLeft);
            Serial.print(" - vertical inclination: ");
            Serial.print(vertInclination);
        }

        // if finished or to close to wall, then stop
        if ((distanceLeft >= -1 and distanceLeft <= 1) or
            currentDistance < US_TOO_CLOSE) {
            // If there is a wall infront, then drive into it
            if (currentDistance <= US_THRESHOLD) {
                Serial.println("Driving Against");
                motor::on(100, true);
                delay(DRIVE_AGAINST_DELAY);
                Serial.println("Driving Away");
                motor::on(100, false);
                delay(DRIVE_AWAY_DELAY);
                // motor::off();
            }
            break;
        }

        delay(ADVANCE_DELAY);
    }

    motor::off();
    return !black_found;
}
