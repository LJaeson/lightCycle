#pragma once
#include <lightCycle/lightCycle.hh>

//Map class
class Map {
private:
    MapTypes::Grid grid;

    //for rending
    // renderQueue rq;
public:
    //constructor
    Map(int w, int h);

    //getter
    MapTypes::Grid getMap() const;

    Tile& getTile(int w, int h);

    Tile& getTile(Location l);

    // void addLocation(Location l) {
    //     rq.addLocation(l);
    // }

    // bool haveLocationTask() {
    //     return rq.haveLocationTask();
    // }

    // Location getLocationQ() {
    //     return rq.getLocationQ();
    // }

};