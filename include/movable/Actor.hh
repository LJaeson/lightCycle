#pragma once
#include <lightCycle/Map.hh>
#include <lightCycle/utility.hh>

class Actor {
protected:
    Position position;
    TileColor actorColor;
    TileColor tileColor;

public:
    Actor(Location l, TileColor ac);

    bool isDead(Map& map);
    void changeTileBehind(Map& map);
    void changeCurrentTile(Map& map);

    void doNextLocation();
    void changeDirection(Direction d);
};
