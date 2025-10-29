#pragma once
#include <lightCycle/lightCycle.hh>
#include <lightCycle/Map.hh>

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

// ---------- Actor ----------
class actor {
protected:
    Position position;
    TileColor actorColor;
    TileColor tileColor;

public:
    actor(Location l, TileColor ac);

    bool isDead(Map& map);
    void changeTileBehind(Map& map);
    void changeCurrentTile(Map& map);

    void doNextLocation();
    void changeDirection(Direction d);
};

class player : public actor {
public:
    player(Location l, TileColor ac);
};

// ---------- Game ----------
class Game {
protected:
    taskQueue tickQueue;
    player p1;
    player p2;
    Map map;
    int terminateCode = 0;

public:
    Game(int w, int h, Location p1Start, Location p2Start);

    void tick();

private:
    void modifyTile_();
    void moveActor_();
    void checkDeath_();

public:
    void draw(sf::RenderTarget& window, int tileSize);
    void drawPart(sf::RenderTarget& window, int tileSize, Location l);

    Map& getMap();
    player& getPlayer1();
    player& getPlayer2();
    int getTerminateCode();

    bool haveLocationTask();
    Location getLocationQ();
};
