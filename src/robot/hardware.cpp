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

namespace motor {

void on(int strength, bool forwards) {
    motorrechtshinten.drehen(strength, forwards);
    motorrechtsvorne.drehen(strength, forwards);
    motorlinkshinten.drehen(strength + LEFT_COMPENSATION, forwards);
    motorlinksvorne.drehen(strength + LEFT_COMPENSATION, forwards);
}

void ramp() { on(); }

void on() { on(MOTOR_SPEED, true); }

void off() {
    motorrechtshinten.drehen(0, true);
    motorrechtsvorne.drehen(0, true);
    motorlinkshinten.drehen(0, true);
    motorlinksvorne.drehen(0, true);
}

void turn(bool right) {
    motorrechtsvorne.drehen(TURN_SPEED, !right);
    motorrechtshinten.drehen(TURN_SPEED, !right);
    motorlinksvorne.drehen(TURN_SPEED, right);
    motorlinkshinten.drehen(TURN_SPEED, right);
}

void turnQuick(bool right) {
    motorrechtsvorne.drehen(TURN_SPEED_QUICK, !right);
    motorrechtshinten.drehen(TURN_SPEED_QUICK, !right);
    motorlinksvorne.drehen(TURN_SPEED_QUICK, right);
    motorlinkshinten.drehen(TURN_SPEED_QUICK, right);
}

} // namespace motor

namespace hardware {

/*
void setup_hardware() {
  DEV_I2C.begin();
  tof.begin();
  tof.VL53L4CX_Off();
  tof.InitSensor(0x12);+
  tof.VL53L4CX_StartMeasurement();
}*/

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
    // NOTE: US_POLL_COUNT needs to be divisible by 2
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

float euclideanDistance(float r1, float g1, float b1, int r2, int g2, int b2) {
    return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

// Color Averages
int redAvg[3] = {94, 32, 21};
float blackAvg[3] = {18, 12, 7};
float whiteAvg[3] = {193, 182, 107}; //{221, 206, 118};
float grayAvg[3] = {136, 105, 61};
float blueAvg[3] = {0, 0, 0};

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
            euclideanDistance(r, g, b, redAvg[0], redAvg[1], redAvg[2]);
        float bDist =
            euclideanDistance(r, g, b, blackAvg[0], blackAvg[1], blackAvg[2]);
        float wDist =
            euclideanDistance(r, g, b, whiteAvg[0], whiteAvg[1], whiteAvg[2]);
        float gDist =
            euclideanDistance(r, g, b, grayAvg[0], grayAvg[1], grayAvg[2]);
        float blueDist =
            euclideanDistance(r, g, b, blueAvg[0], blueAvg[1], blueAvg[2]);

        if (PRINT_RGB_VALUES) {
            Serial.println(rDist);
            Serial.println(bDist);
            Serial.println(wDist);
            Serial.println(gDist);
        }
        float smallest = minimum(rDist, bDist, wDist, gDist, blueDist);
        if (smallest == rDist) {
            Serial.println("   Detected Victim");
            return TileType::Victim;
        } else if (smallest == bDist) {
            Serial.println("   Detected Black");
            return TileType::Black;
        } else if (smallest == blueDist) {
            Serial.println("   Detected Blue");
            return TileType::Blue;
        } else {
            Serial.println("   Detected Normal");
            return TileType::Normal;
        }
    }
}

void ejectRescuePacket() {
    Serial.println("Ejecting rescue kit...");

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
}

CorrectOrientationResult correctOrientation(int targetOrientation, bool quick) {
    auto isCorrect = [](int c, int g) {
        int diff1 = abs(c - g);
        int diff2 = 360 - diff1;
        int diff = diff1 < diff2 ? diff1 : diff2; // min
        return diff <= TURN_TOL;
    };

    CorrectOrientationResult res;
    res.wasWrong = false;

    sensors_event_t event;
    int currentOrientation;

    while (true) {
        bno.getEvent(&event);
        currentOrientation = event.orientation.x;

        if (PRINT_TURNING_PROGRESS) {
            Serial.print("   turn: ");
            Serial.println(currentOrientation);
        }

        if (isCorrect(currentOrientation, targetOrientation))
            break;

        Serial.print("Correcting orientation: ");

        bool dir = getTurnDir(currentOrientation, targetOrientation);

        Serial.print(dir);

        if (quick) {
            motor::turnQuick(dir);
        } else {
            motor::turn(dir);
        }

        if (!quick) {
            delay(TURN_DELAY + 30);
            motor::off();
            delay(10);
        }
        delay(TURN_DELAY);
    }

    if (quick) {
        motor::off();
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

    Serial.print(startingOrientation);
    Serial.print(" to ");
    Serial.println(goalOrientation);

    correctOrientation(goalOrientation, true);
    correctOrientation(goalOrientation, false);

    motor::off();
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
    Serial.print("history amp: ");
    Serial.println(maxVal - minVal);
    return ((maxVal - minVal) < US_MAX_AMPLITUDE) and (minVal < 60);
}

void unstuck() {
    Serial.println("Moving back");
    motor::on(MOTOR_SPEED, false);
    delay(US_MAX_AMPLITUDE_TIME);
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
    bool badDriving = startingDistance > MAX_CORRECT_MEASUREMENT;

    int vertInclination = 0;
    bool ignore = false;

    bool doModulo = false;

    // sensors_event_t event;

    Serial.print("Advancing at distance ");
    Serial.print(startingDistance);
    Serial.println("...");

    motor::on();
    if (TESTING_VIRTUAL) {
        delay(VIRT_DELAY);
    }

    while (true and !TESTING_VIRTUAL) {

        // fix orientation and get orientation
        auto res = correctOrientation(getDegrees(rd, RelDir::Front), false);
        // measure inclination
        double zOr = res.event.orientation.z;
        double yOr = res.event.orientation.y;
        vertInclination = abs(yOr);

        // check if on ramp
        if (vertInclination >= RAMP_INCLINATION) {
            ignore = true;
            badDriving = false;
            Serial.println("   Ramp Detected!!!");
        }

        Serial.println(zOr);
        Serial.println(yOr);
        if (yOr <= -RAMP_INCLINATION) {
            Serial.println("Ignoring Up Ramp");
            motor::on(MOTOR_SPEED, false);
            delay(1750);
            ignore = false;
            black_found = true;
            break;
        }

        if (ignore) {
            motor::ramp();
        }

        // check tile color
        if (!ignore) {
            TileType tt = getFloorTileType();
            if (tt == TileType::Black) {
                // drive back to start
                goalDistance = startingDistance;
                black_found = true;
            } else if (tt == TileType::Victim) {
                if (!victimDiscoveredLastTile) {
                    victimDiscovered = true;
                    Serial.println("Victim marked as discovered!");
                }
            }
        }

        int currentDistance =
            usf.auslesen(); // getUltrasonicMedian(&usf);//usf.auslesen();

        int distanceLeft = currentDistance - goalDistance;
        // if (abs(distanceLeft-lastDistanceLeft) > DISTANCE_LEFT_TOL) {
        // badDriving = true;
        //}

        if ((distanceLeft > 39) and !black_found) {
            doModulo = true;
        }
        if (doModulo) {
            distanceLeft = distanceLeft % TILE_SIZE;
        }

        shiftArray(history, US_HISTORY_SIZE);
        history[US_HISTORY_SIZE - 1] = currentDistance;
        if (isNotMoving(history) and !ignore and !black_found) {
            unstuck();
            Serial.println("because of history");
            history[0] = 10;
            history[1] = 30;
            history[2] = 50;
            history[3] = 70;
            history[4] = 90;
        }

        // update direction (forward/backward)
        if (!ignore) {
            motor::on(MOTOR_SPEED, distanceLeft > 0);
        }

        if (PRINT_ADVANCE_PROGRESS) {
            int dif = startingDistance - currentDistance;
            Serial.print("   d: ");
            Serial.print(currentDistance);
            Serial.print(" - left: ");
            Serial.print(distanceLeft);
            Serial.print(" - vinc: ");
            Serial.print(vertInclination);
        }
        if (badDriving) {
            for (int i = 0; i < 15; i++) {
                delay(BAD_DRIVING_CYCLE_TIME);
                auto result =
                    correctOrientation(getDegrees(rd, RelDir::Front), false);
                if (result.wasWrong) {
                    i--;
                }
                motor::on();

                TileType tt = getFloorTileType();
                if (tt == TileType::Black) {
                    // drive back to start
                    goalDistance = startingDistance;
                    black_found = true;
                } else if (tt == TileType::Victim) {
                    if (!victimDiscoveredLastTile) {
                        victimDiscovered = true;
                    }
                }
                if (black_found) {
                    motor::on(MOTOR_SPEED, false);
                    for (int j = i; j > 0; j--) {
                        getFloorTileType();
                    }
                    // delay(1000);
                    // delay((i+1)*BAD_DRIVING_CYCLE_TIME);
                    break;
                }
                Serial.print(i);
            }
            break;
        } else {
            // if finished or to close to wall, then stop
            if (((distanceLeft >= -1 and distanceLeft <= 1) and !ignore) or
                currentDistance < US_TOO_CLOSE) {
                // If there is a wall infront, then drive into it
                if (currentDistance <= US_THRESHOLD) {
                    Serial.println("Driving Against");
                    motor::on(100, true);
                    delay(DRIVE_AGAINST);
                    Serial.println("Driving Away");
                    motor::on(100, false);
                    delay(DRIVE_AWAY);
                    // motor::off();
                }
                break;
            }
        }

        delay(ADVANCE_DELAY);
    }

    if (ignore) { // was on ramp
        // for the very rare edge case with ramps where the robot falsely
        // measures black right infront of the ramp. There is no way to fix this
        // bug except changing ramp detection. However, this edge case has a sub
        // edge case (that has an even worse outcome) that we can detect If,
        // after the false black detection, a ramp is detected in the next cycle
        // (because the motors didn't stop in time) the robot will continue
        // going down the ramp (because ignore == true) but success will have
        // been set to false (because of the black tile), so the robot will go
        // down the ramp but not update its position (because the position
        // update depends on success)

        // this sub edge case is detected here (if ignore is true, the robot
        // must have been a ramp, which means we should ignore any black tile
        // detection = set success to true) the same thing also goes for red
        // tiles (though the consequences aren't as grave), so reset them as
        // well

        // success = true;
        black_found = false;

        victimDiscovered = false;
        victimDiscoveredLastTile = false;
    }

    motor::off();
    return !black_found;
}

} // namespace hardware
