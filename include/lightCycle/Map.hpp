#pragma once
#include <lightCycle/lightCycle.hh>

//Map class
class Map {
private:
    MapTypes::Grid grid;

public:
    //constructor
    Map(int w, int h);

    //getter
    MapTypes::Grid getMap() const;

    Tile& getTile(int w, int h);

    Tile& getTile(Location l);

};
