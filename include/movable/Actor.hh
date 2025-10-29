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

    virtual bool isPlayer() {return false;};
    virtual bool isBot() {return false;};
    TileColor getColor() const { return actorColor; };

    //dummy, only useful for bot
    virtual Direction getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
        return Direction::UP; 
    }
};
