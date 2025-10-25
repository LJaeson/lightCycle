#pragma once
#include <lightCycle/lightCycle.hh>

class Player {
    public:
        virtual Direction getMove() = 0;
        virtual bool clientControlled() = 0;
};

class Client: public Player {
    public:
        virtual bool clientControlled() override { return true; }
        virtual Direction getMove() override { return Direction::DOWN; }
};

