#pragma once
#include <movable/Actor.hh>
#include <lightCycle/utility.hh>
#include <lightCycle/GameState.hh>

class Bot : public Actor {
public:
    Bot(Location l, TileColor ac);

    bool isBot() override {return true;};

private:
    GameState tempGameState;
    double timeLimit;
public:
    Bot(Location l, TileColor ac);
    // Bot(int w, int h);

    // bool clientControlled() override;

    Direction getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

    bool botCanNotMove();
    bool oponentCanNotMove();

    bool isTerminal();

    double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position);

    double evaluate();

    bool exceedTimeLimit(sf::Clock &clock);

    bool isDraw();

};
