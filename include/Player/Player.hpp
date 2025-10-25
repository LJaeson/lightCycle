#pragma once
#include <lightCycle/lightCycle.hh>

class Player {
    public:
        virtual Direction getMove(Game game, TileColor color1, TileColor color2, double timeLimit) = 0;
        virtual bool clientControlled() = 0;
};

class Client: public Player {
    public:
        virtual bool clientControlled() override { return true; }
        virtual Direction getMove(Game game, TileColor color1, TileColor color2, double timeLimit) override { return Direction::DOWN; }
};
