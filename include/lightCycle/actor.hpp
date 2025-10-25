#pragma once
#include <lightCycle/Map.hpp>
#include <lightCycle/lightCycle.hh>

//actor class
class actor {
private:
    Position position;
    TileColor actorColor;
    TileColor tileColor;
public:
    //constructor

    actor(Location l, TileColor ac);

    //function
    bool isDead(Map& map);

    void changeTileBehind(Map& map);

    void changeCurrentTile(Map& map);

    //accessor
    void doNextLocation();

    void changeDirection(Direction d);

};

class player : public actor{
private:
public:
    //constructor

    player(Location l, TileColor ac) : actor(l, ac) {}

    //function

    // void 

};