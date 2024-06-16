#include "maze.h"
#include "error.h"
#include <Arduino.h>

/* Current Walls */

bool CurrentWalls::get(RelDir relDir) {
    switch (relDir) {
    case RelDir::Front:
        return data.front;
    case RelDir::Right:
        return data.right;
    case RelDir::Left:
        return data.left;
    default:
        raise("Tried to get wall at invalid direction");
    }
}

void CurrentWalls::set(RelDir relDir, bool value) {
    switch (relDir) {
    case RelDir::Front:
        data.front = value;
        break;
    case RelDir::Right:
        data.right = value;
        break;
    case RelDir::Left:
        data.left = value;
        break;
    default:
        if (TESTING_VIRTUAL) {
            break;
        }
        raise("Tried to set wall at invalid direction");
    };
}

/* Maze Platform */
/*
Tile *MazePlatform::get(int x, int y) {

}

bool MazePlatform::inBounds(int x, int y) {
    if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) {
        return false;
    }
    return true;
}

bool MazePlatform::inBounds(int x, int y, CardDir cardDir) {
   shift(x, y, cardDir);
   return inBounds(x, y);
}

Tile *MazePlatform::get(int x, int y, CardDir cardDir) {
    if (!inBounds(x, y, r)) {
        raise("Tried to get out of bounds tile");
    }
    if (!inBounds(x, y, cardDir)) {
            raise("Tried to get out of bounds tile next to tile in bounds");

    }

    shift(x, y, cardDir);
    return &data[x][y];
}
*/

/* Maze */

/*MazePlatform *Maze::getCurrentMazePlatform() {
    if (currentPlatform == 0) {
        return &maze0;
    } else {
        raise("maze 1 accessed");
        //return &maze1;
    }
}*/

Tile *Maze::get(int x, int y) {
    if (!inBounds(x, y)) {
        raise("Tried to get out of bounds tile");
    }
    return &(data[x][y]);
}

bool Maze::inBounds(int x, int y) {
    if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) {
        return false;
    }
    return true;
}

Tile *Maze::getInDir(int x, int y, CardDir cardDir) {
    shift(x, y, cardDir);
    return get(x, y);
}

bool Maze::inBoundsInDir(int x, int y, CardDir cardDir) {
    shift(x, y, cardDir);
    // shiftRamp(x, y, layer, cardDir);
    return inBounds(x, y);
}

/*
void shiftRamp(int &x, int &y, int &l, CardDir dir) {
    // check if position is at ramp and showing in ramps direction, and if yes,
shift to end of ramp if (l == 0 and maze.ramp.x0 == x and maze.ramp.y0 == y) {
        if (dir == maze.ramp.dir0) {
            x = maze.ramp.x1;
            y = maze.ramp.y1;
            l = 1;
            return;
        }
    }
    if (l == 1 and maze.ramp.x1 == x and maze.ramp.y1 == y) {
        if (dir == maze.ramp.dir1) {
            x = maze.ramp.x0;
            y = maze.ramp.y0;
            l = 0;
            return;
        }
    }
    shift(x, y, dir);
}
*/

bool setPredeterminedWalls(int x, int y, CurrentWalls *walls,
                           TileType *currentTileType) {
    // Define a struct to hold predetermined wall data for various positions
    struct PredeterminedWall {
        int x;
        int y;
        bool up;
        bool right;
        bool down;
        bool left;
        TileType tileType;
    };

    // Define an array of predetermined walls
    PredeterminedWall predeterminedWalls[] = {
        {5, 5, false, true, true, false, TileType::Normal},
        {5, 4, true, false, false, true, TileType::Victim},
        {6, 4, false, false, true, false, TileType::Normal},
        {7, 4, true, true, true, false, TileType::Normal},
        {6, 3, true, true, false, true, TileType::Normal},
        {1, 4, false, false, false, false, TileType::Normal},
        {2, 4, false, false, false, false, TileType::Normal},
        {2, 3, false, false, false, false, TileType::Normal},
        {4, 6, false, false, false, false, TileType::Normal},
        {2, 6, false, false, false, false, TileType::Normal},
        {3, 6, false, true, true, true, TileType::Normal},
        {4, 5, false, false, true, false, TileType::Normal},
        {3, 5, false, false, false, true, TileType::Normal},
        {3, 4, false, false, false, true, TileType::Normal},
        {3, 3, true, true, false, true, TileType::Normal},
        {3, 2, true, false, true, true, TileType::Normal},
        {4, 2, true, false, false, false, TileType::Normal},
        {5, 2, true, false, false, false, TileType::Normal},
        {6, 2, true, false, true, false, TileType::Normal},
        {7, 2, true, false, false, false, TileType::Normal},
        {8, 2, true, false, false, false, TileType::Normal},
        {9, 1, false, false, false, false, TileType::Normal},
        {9, 2, true, true, false, false, TileType::Normal},
        {9, 3, false, true, false, false, TileType::Normal},
        {9, 4, false, true, false, true, TileType::Normal},
        {9, 5, false, true, false, false, TileType::Normal},
        {9, 6, false, true, true, false, TileType::Normal},
        {8, 6, true, false, true, true, TileType::Normal},
        {8, 5, false, false, true, true, TileType::Normal},
        {8, 4, true, true, false, false, TileType::Normal},
        {8, 3, false, false, true, false, TileType::Normal},
        {7, 3, false, false, true, true, TileType::Normal},
        {5, 3, false, true, true, false, TileType::Normal},
        {4, 4, false, true, false, false, TileType::Normal},
        {4, 3, false, false, false, true, TileType::Normal}};

    // Iterate through the predetermined walls data
    for (const PredeterminedWall &wall : predeterminedWalls) {
        if (x == wall.x && y == wall.y) {
            // Set the walls based on the matching predetermined wall
            walls->set(toRel(CardDir::Up, rd), wall.up);
            walls->set(toRel(CardDir::Down, rd), wall.down);
            walls->set(toRel(CardDir::Left, rd), wall.left);
            walls->set(toRel(CardDir::Right, rd), wall.right);
            *currentTileType = wall.tileType;

            Serial.print("Wall Programmed: ");
            Serial.println(wall.up);
            Serial.print("Robot Direction: ");
            Serial.println(static_cast<int>(rd));
            if (static_cast<int>(toRel(CardDir::Up, rd)) !=
                static_cast<int>(RelDir::Back)) {
                Serial.print("Wall Exists: ");
                Serial.println(walls->get(toRel(CardDir::Up, rd)));
            }
            return true; // Return true if predetermined walls are set
        }
    }

    // Default, when tile isn't preprogrammed
    walls->set(toRel(CardDir::Up, rd), false);
    walls->set(toRel(CardDir::Down, rd), false);
    walls->set(toRel(CardDir::Left, rd), false);
    walls->set(toRel(CardDir::Right, rd), false);
    *currentTileType = TileType::Normal;

    return true; // Return false if no predetermined walls match
}
