#pragma once
#include <lightCycle/Map.hh>
#include <lightCycle/utility.hh>
#include <string>


class Game;  // forward declaration

#define BOT_OWNER 2
#define OPP_OWNER 1
#define EQUAL_OWNER 0 
#define UNVISIT -1

struct BotPlayingReturn {
    Direction dir;
    int positionCount;
    int time; // as millisecond

    std::string returnString() {
        return "Position count: " + std::to_string(positionCount) + ", Time taken: " + std::to_string(time) + " ms\n";
    }
};

struct ComponentInfo {
    int distToNearestPlayer;
    int owner;
};

class Actor {
protected:
    Position position;
    TileColor actorColor;
    TileColor tileColor;

public:
    virtual ~Actor() = default;

    Actor(Location l, TileColor ac);

    bool isDead(Map& map);
    void changeTileBehind(Map& map);
    void changeCurrentTile(Map& map);

    void doNextLocation();
    void changeDirection(Direction d);

    bool equalLocation(Actor &a);

    virtual bool isPlayer() {return false;};
    virtual bool isBot() {return false;};
    TileColor getColor() const { return actorColor; };

    //dummy, only useful for bot
    virtual BotPlayingReturn getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
        return { Direction::UP, 0, 0}; 
    }
};
