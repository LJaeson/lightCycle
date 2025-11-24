#pragma once
#include <functional>
#include <lightCycle/Map.hh>
#include <lightCycle/utility.hh>
#include <movable/Player.hh>
#include <memory>
#include <iostream>

// #include <deque>
// #include <iostream>

// ---------- TaskQueue ----------
class taskQueue {
protected:
    using Task = std::function<void()>;
    std::deque<Task> task_;
    std::deque<Location> location_task_;
public:
    void addTask(Task fn) { task_.push_front(fn); }

    void executeTick() {
        while (!task_.empty()) {
            Task fn = task_.back();
            task_.pop_back();
            fn();
        }
    }

    void addLocation(Location l) { location_task_.push_front(l); }
    bool haveLocationTask() { return !location_task_.empty(); }
    Location getLocationQ() {
        Location l = location_task_.back();
        location_task_.pop_back();
        return l;
    }
};


// ---------- Game ----------
class Game {
protected:
    taskQueue tickQueue;
    // Player p1;
    // Bot p2;
    std::unique_ptr<Actor> p1;
    std::unique_ptr<Actor> p2;
    Map map;
    int terminateCode = 0;

    //temp, need to relocated
    // const double BOT_LIMIT = 700.0;

public:
    Game(int w, int h, Location p1Start, Location p2Start);

    void tick();

private:
    void modifyTile_();
    void moveActor_();
    void checkDeath_();
    void botPlaying_();

public:
    void draw(sf::RenderTarget& window, int tileSizeW, int tileSizeH);
    void drawPart(sf::RenderTarget& window, int tileSizeW, int tileSizeH, Location l);

    Map& getMap();
    Actor& getPlayer1();
    Actor& getPlayer2();
    int getTerminateCode();

    bool haveLocationTask();
    Location getLocationQ();

    MapTypes::Grid getMapGrid() const {
        return map.getMap();
    }
};
