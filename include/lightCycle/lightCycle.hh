#pragma once
#include <stdio.h>
#include <filesystem>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include <vector>
#include <deque>

#include <functional>


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
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};

//positioning
struct Location {
    int w;
    int h;

    int getW() {
        return w;
    }

    int getH() {
        return h;
    }

    bool equal(const Location &l) const {
        return w == l.w && h == l.h;
    }
};

struct Position {
    Location location;

    int dw[4]  = {0, 1, 0, -1};
    int dh[4]  = {-1, 0, 1, 0};

    //0 <= direction <= 3. which is indicated as UP,RIGHT, DOWN, LEFT
    Direction direction;

    void initPosition(int w, int h) {
        location = Location{w, h};
        direction = Direction::DOWN;
    }

    void initPosition(Location l) {
        location = l;
        direction = Direction::DOWN;
    }

    bool changeDirection(Direction d) {
        if (d == Direction::DOWN && direction == Direction::UP) return false;
        if (d == Direction::UP && direction == Direction::DOWN) return false;
        if (d == Direction::RIGHT && direction == Direction::LEFT) return false;
        if (d == Direction::LEFT && direction == Direction::RIGHT) return false;
        direction = d;
        return true;
    }

    Location findNextLocation() const {
        return Location{location.w + dw[direction], location.h + dh[direction]};
    }

    Location findPreLocation() const {
        return Location{location.w - dw[direction], location.h - dh[direction]};
    }

    Location findNextLocationFromDirection(Direction d) {
        return Location{location.w + dw[d], location.h + dh[d]};
    }

    void doNextLocation() {
        location = findNextLocation();
    }

    void doPreLocation() {
        location = findPreLocation();
    }
};

//map
struct Tile {
    Location location;
    TileColor tileColor;

    void changeTileColor(TileColor ac) {
        tileColor = ac;
    }
};

namespace MapTypes {
    using Grid = std::vector<std::vector<Tile>>;
}
