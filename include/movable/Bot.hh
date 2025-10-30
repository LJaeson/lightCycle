#pragma once
#include <movable/Actor.hh>
#include <lightCycle/utility.hh>
// #include <Clock.hpp>
#include <SFML/System.hpp>
// #include <lightCycle/lightCycle.hh>
// #include <Clock.hpp>
#include <lightCycle/GameState.hh>

class GameState;

class Bot : public Actor {
public:
    Bot(Location l, Location mapSize, TileColor ac);

    bool isBot() override {return true;};

private:
    std::unique_ptr<GameState> tempGameState;
    double timeLimit;
public:


    Direction getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

    bool botCanNotMove();
    bool oponentCanNotMove();

    bool isTerminal();

    double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position);

    double virtual evaluate();

    bool exceedTimeLimit(sf::Clock &clock);

    bool isDraw();

};
