
# <div style="display:flex; vertical-align: middle; align-items: center; width: 100%; justify-content: center;"><span>Stahl<sup>3</sup></span></div>

<div style="display:flex; vertical-align: middle; align-items: center; width: 100%; justify-content: center;"><span>Our robot for the RoboCup</span></div>

## What is the RoboCup?
The RoboCup is an international competition, where teams from all over the world meet and develop robots in different leagues. We are competing in the RoboCup Junior Rescue Maze league, where we need to build and program a robot that traverses a maze with obstacles and ramps while reacting to colored tiles on the floor. 

## Our Accomplishments
### Rescue Maze Entry
- ?th place in RoboCup Junior Vöhringen 2023
- ?th place in RoboCup Junior Vöhringen 2024
- ?th place in RoboCup Junior German Open 2024


## How the robot works
### Sensors:
- 3 **ultrasonic sensors**, on the front and sides, to determine wall placement
- 1 **color sensor** to deterine tile color
- 1 **magnetic field sensor** to determine orientation (both horizontal for accurate turning, and vertical for detection of ramps)
### Other Equipment:
- 1 **Servo** to eject rescue kits
- 1 **LED** to signal victim recognition
- motors + drivers
### Basics
The robot repeats the following algorithm until it reaches the start of the maze again:
- Read sensor data
- Take action (e.g. eject a rescue kit if the robot is standing on a red tile)
- Determine next tile to visit
- Turn to direction of tile
- Advance 1 tile

We determine the next tile using a version of [depth-first search](https://en.wikipedia.org/wiki/Depth-first_search). The robot drives through the maze until it hits upon a dead end. It then turns around and drives until it arrives at the next junction (or the beginning of the maze), after which the algorithm repeats. This allows us to explore the whole maze in a small number of steps and to return to the start of the maze at the end.

For this algorithm we need to keep track of which tiles we have visited. This is done by storing a matrix of all tiles, and updating it whenever we visit a new tile. Additionally, we save the direction that we came from so that we can drive back that way when we arrive at a dead end.

## Building
This project has multiple dependencies:
- [Adafruit_BNO055](https://github.com/adafruit/Adafruit_BNO055)
- [Adafruit_TCS34725](https://github.com/adafruit/Adafruit_TCS34725)
- [Servo](https://www.arduino.cc/en/Reference/Servo)

These can be installed using the library manager of the Arduino IDE or the Arduino VSCode extension. For manually installing the dependencies please note that these subdependencies are also required:
- [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO)
- [Adafruit_Unified_Sensor](https://github.com/adafruit/Adafruit_Sensor)

Additionally these custom libraries are required:
- ka03_driver
- ultraschall

These can be found under `src/lib/` in this project. To install them you need to copy them to Arduino's library path. For Linux this is located at `~/Arduino/libaries/`.

You can use this command to copy them:
```sh
cp -r src/lib/* ~/Arduino/libraries/
```

Once you have installed the required libraries, you can open the project using the Arduino IDE and compile it.
