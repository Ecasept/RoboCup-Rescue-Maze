#pragma once
#include "common.h"
#define MAZE_SIZE 30

class CurrentWalls {
    // NOTE: bit field is apparently not initialized by default, so will contain
    // random data before measuring
  public:
    struct {
        bool front : 1;
        bool right : 1;
        bool left : 1;
    } data;

    bool get(RelDir relDir);
    void set(RelDir relDir, bool value);
};

class Tile {
  public:
    // If the tile has been visited
    bool visited = false;
    CardDir origin; // The direction that the robot came from when it entered this tile for the first time
    bool isStartingTile = false;
};

/*
class MazePlatform {
    public:
            Tile data[MAZE_SIZE][MAZE_SIZE]; // [rows][columns]
            // returns the tile at x, y
            //Tile *get(int x, int y);
            //Tile *get(int x, int y, CardDir cardDir);
            //bool inBounds(int x, int y);
            //bool inBounds(int x, int y, CardDir cardDir);
};
*/

class Maze {
    private:
        //MazePlatform maze0;
        //MazePlatform maze1;
        Tile data[MAZE_SIZE][MAZE_SIZE]; // [rows][columns]
        // returns the tile at x, y
    public:
        Tile *get(int x, int y);
        Tile *getInDir(int x, int y, CardDir cardDir);
        bool inBounds(int x, int y);
        bool inBoundsInDir(int x, int y, CardDir cardDir);
};

bool setPredeterminedWalls(int x, int y, CurrentWalls *walls, TileType *currentTileType);

//void shiftRamp(int &x, int &y, int &l, CardDir dir);

extern Maze maze;
