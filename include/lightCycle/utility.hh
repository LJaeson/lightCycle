#pragma once
enum TileColor {
    NOPE,
    BLUE,
    GREEN,
    BLUEACTOR,
    GREENACTOR,
    BOUNDARY,
    LIGHTBLUE,
    LIGHTGREEN
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

    int getW() {
        return w;
    }

    int getH() {
        return h;
    }

    bool equal(Location l) {
        return (w == l.getW()) && (h == l.getH());
    }
};

extern int dw[4];
extern int dh[4];

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
        if (d == Direction::DOWN && direction == Direction::UP) return;
        if (d == Direction::UP && direction == Direction::DOWN) return;
        if (d == Direction::RIGHT && direction == Direction::LEFT) return;
        if (d == Direction::LEFT && direction == Direction::RIGHT) return;
        direction = d;
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