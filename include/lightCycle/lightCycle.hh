#include <stdio.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include <vector>
#include <deque>


enum TileColor {
    NOPE,
    BLUE,
    GREEN,
    BLUEACTOR,
    GREENACTOR,
    BOUNDARY
};



//     1
// 4       2
//     3
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

//positioning
struct Location {
    int w;
    int h;
};

int dw[4]  = {0, 1, 0, -1};
int dh[4]  = {-1, 0, 1, 0};

struct Position {
    Location location;

    //0 <= direction <= 3. which is indicated as UP,RIGHT, DOWN, LEFT
    int direction;

    void initPosition(int w, int h) {
        location = Location{w, h};
        direction = Direction::DOWN;
    }

    void initPosition(Location l) {
        location = l;
        direction = Direction::DOWN;
    }

    void changeDirection(Direction d) {
        direction = d;
    }

    Location findNextLocation() const {
        return Location{location.w + dw[direction], location.h + dh[direction]};
    }

    Location findPreLocation() const {
        return Location{location.w - dw[direction], location.h - dh[direction]};
    }

    void doNextLocation() {
        location = findNextLocation();
    }
};

//map
struct Tile{
    Location location;
    TileColor tileColor;

    void changeTileColor(TileColor ac) {
        tileColor = ac;

    }
};

namespace MapTypes {
    using Grid = std::vector<std::vector<Tile>>;
    

}
