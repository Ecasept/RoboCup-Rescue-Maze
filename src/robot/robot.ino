#include "error.h"
#include "hardware.h"
#include "maze.h"

// DOESN'T MATTER: TODO: better debug
// victimDisovered fix bug
// FIXED! is backwards driving bug fixed/does the feature even work?
// 180 after black tile
Maze maze;
CurrentWalls currentWalls;
int robotX = START_X;
int robotY = START_Y;
CardDir rd; // Robot direction
TileType currentTileType;
Tile *currentTile;
bool victimDiscovered = false;

bool firstTimeLoop = true;

void loop() {
    /*digitalWrite(VICTIM_PIN, HIGH);
    delay(1000);
    digitalWrite(VICTIM_PIN, LOW);
    delay(1000);
    return;*/
    // sensors_event_t event;
    // bno.getEvent(&event);
    // Serial.println("y/z");
    //  y: -

    // Serial.println(event.orientation.y);
    // Serial.println(event.orientation.z);
    // return;
    // Serial.println(hardware::getUltrasonicMedian(&usf));
    // delay(100);
    // return;
    // testColorSensor();
    // motor::on(255, false);
    // return;
    // getAverages();
    // delay(10000);
    // return;
    /*while (true) {
    motor::on(MOTOR_SPEED, false);
    //Serial.println(usr.auslesen());
    }*/

    // while(true) {Serial.println(usr.auslesen()); delay(100);}
    // testColorSensor();
    // testMotors();-9*5

    // delay(10000000);
    // hardware::advance();
    // while(true){hardware::ejectRescuePacket();delay(1000);}

    if (firstTimeLoop) {
        firstTimeLoop = false;
        Serial.println("Entered Loop!");
        maze.get(robotX, robotY)->isStartingTile = true;
    } else {
        Serial.println("\n\n\n\n\n\n\n");
    }

    // In the beginning, each tile is `unvisited`, once visited `visited`, and
    // once all branches are explored `explored` `free` == no wall
    // NOTE: I ~think~ know explored isn't even used anymore
    scanSurroundings();
    if ((currentTileType == TileType::Victim or victimDiscovered) and
        !maze.get(robotX, robotY)->visited) {
        // motorOn(MOTOR_SPEED, true);
        hardware::ejectRescuePacket();
        victimDiscovered = true;
    }

    auto canGo = [](RelDir relDir) {
        Serial.print(dstr(relDir));
        if (currentWalls.get(relDir)) {
            Serial.println(": Wall Exists");
            return false;
        }

        if (maze.getInDir(robotX, robotY, toCard(relDir, rd))->visited) {
            Serial.println(": Already Visited");
            return false;
        }

        Serial.println(": Success");
        return true;
    };

    RelDir direction;
    bool dirFound = false;

    RelDir dirs[] = {RelDir::Front, RelDir::Right, RelDir::Left};
    for (const auto &dir : dirs) {
        if (canGo(dir)) {
            direction = dir;
            // set origin
            if (not currentTile->visited) {
                currentTile->origin = toCard(RelDir::Back, rd);
                Serial.print("Setting origin to: ");
                Serial.println(dstr(currentTile->origin));
            }
            dirFound = true;
            break;
        }
    }

    if (!dirFound) {
        if (currentTile->visited) {
            if (currentTile->isStartingTile) {
                Serial.println("\nMaze Completed!\n");
                delay(10000);
            }
            // Backtrack
            Serial.print("Backtracking - following origin: ");
            Serial.println(dstr(currentTile->origin));
            direction = toRel(currentTile->origin, rd);
        } else {
            // Turn around
            direction = RelDir::Back;
        }
    }

    currentTile->visited = true;

    if (direction == RelDir::Back) {
        hardware::turn(RelDir::Right);
        hardware::turn(RelDir::Right);
    } else {
        hardware::turn(direction);
    }
    bool success = hardware::advance();

    if (success) {
        shift(robotX, robotY, rd);
    } else {
        maze.getInDir(robotX, robotY, rd)->visited = true;
        Serial.println("success was false, setting black tile to visited");
        hardware::turn(RelDir::Right);
        hardware::turn(RelDir::Right);
    }
}

void setup() {
    Serial.begin(9600);
    Serial.print("\nSetting up... ");
    if (!bno.begin()) {
        raise("\nCould not start magnetic field sensor");
    }
    if (!colorsensor.begin()) {
        raise("\nCould not start color sensor");
    }
    delay(1000);
    bno.setExtCrystalUse(true);

    ejectServo.attach(23);
    ejectServo.write(180);

    pinMode(VICTIM_PIN, OUTPUT);

    Serial.println("Finished!");
}

void scanSurroundings() {
    Serial.print("Scanning ( ");
    Serial.print(robotX);
    Serial.print(" | ");
    Serial.print(robotY);
    Serial.print(" )");
    currentTile = maze.get(robotX, robotY);

    auto frontSpace = hardware::getUltrasonicMedian(&usf);
    auto rightSpace = hardware::getUltrasonicMedian(&usr);
    auto leftSpace = hardware::getUltrasonicMedian(&usl);

    Serial.println(" - done!");
    if (TESTING_VIRTUAL) {
        bool wallDefined = setPredeterminedWalls(robotX, robotY, &currentWalls,
                                                 &currentTileType);
        if (!wallDefined) {
            raise(String("No predetermined wall defined at " + String(robotX) +
                         String(", ") + String(robotY))
                      .c_str());
        }
    } else {
        currentWalls.data.front = frontSpace <= US_THRESHOLD;
        currentWalls.data.right = rightSpace <= US_THRESHOLD;
        currentWalls.data.left = leftSpace <= US_THRESHOLD;
        currentTileType = hardware::getFloorTileType();
    }

    Serial.print("Ultrasonic (tol>");
    Serial.print(US_THRESHOLD);
    Serial.println("):");
    Serial.print("  ");
    Serial.println(frontSpace);
    Serial.print(leftSpace);
    Serial.print("  ");
    Serial.println(rightSpace);
    Serial.println("");
}

void getAverages() {
    // Measure colorsensor `iter` times and print average rgbc values
    int sR = 0; // red sum
    int sG = 0; // green sum
    int sB = 0; // blue sum
    int sC = 0; // clear sum
    int iter = 100;

    for (int i = 0; i < iter; i++) {
        Serial.print(i + 1);
        Serial.print("/");
        Serial.println(iter);
        uint16_t r, g, b, c;
        // colorsensor.setInterrupt(false); // led
        colorsensor.getRawData(&r, &g, &b, &c);
        sR = sR + r;
        sG = sG + g;
        sB = sB + b;
        sC = sC + c;
    }
    Serial.print("Averages: r=");
    Serial.print(sR / iter);
    Serial.print(" g=");
    Serial.print(sG / iter);
    Serial.print(" b=");
    Serial.print(sB / iter);
    Serial.print(" c=");
    Serial.println(sC / iter);
}

void testColorSensor() {
    while (true) {
        hardware::getFloorTileType();
    }
}

void testMotors(bool async = false) {
    motor::on(MOTOR_SPEED, true);
    if (!async) {
        while (true) {
            delay(100000);
        }
    }
}
