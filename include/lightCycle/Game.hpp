#pragma once
#include <lightCycle/lightCycle.hh>
#include <lightCycle/Map.hpp>
#include <lightCycle/actor.hpp>

// class renderQueue {
// protected:

//     std::deque<Location> location_task_;
// public:
//     void addLocation(Location l) {
//         location_task_.push_front(l);
//     }

//     bool haveLocationTask() {
//         if(location_task_.empty()) {
//             return false;
//         }
//         return true;
//     }

//     Location getLocationQ() {
//         Location l = location_task_.back();
//         location_task_.pop_back();
//         return l;
//     }
// };

//taskQueue
class taskQueue {
protected:
    using Task = std::function<void()>;

    std::deque<Task> task_;
    std::deque<Location> location_task_;
public:
    void addTask(Task fn);

    void executeTick();

    void addLocation(Location l);

    bool haveLocationTask();

    Location getLocationQ();
};

//game class, the big class that contain everything about game
class Game {
protected:
    taskQueue tickQueue;
    //for now, there is only human player, no npc, so we make two player
    player  p1;
    player  p2;

    Map map;

    int terminateCode = 0;

public:
    //constructor to init the game
    Game(int w, int h, Location p1Start, Location p2Start);

    //tick
    void tick();

    //tickfunction
    void modifyTile_();

    void moveActor_();

    void checkDeath_();

    //accessor
    Map& getMap();

    player& getPlayer1();

    player& getPlayer2();
    
    int getTerminateCode();

    // bool haveLocationTask() {
    //     return map.haveLocationTask();
    // }

    // Location getLocationQ() {
    //     return map.getLocationQ();
    // }

    MapTypes::Grid getMapGrid();

    void changeTileColor(Location l, TileColor c);
};