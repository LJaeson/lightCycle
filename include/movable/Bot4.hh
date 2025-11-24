#pragma once
#include <movable/Actor.hh>
#include <lightCycle/utility.hh>
// #include <Clock.hpp>
#include <SFML/System.hpp>
#include <lightCycle/GameState.hh>
// #include <lightCycle/lightCycle.hh>
// #include <Clock.hpp>

class Bot4 : public Actor {
public:
    Bot4(Location l, Location mapSize, TileColor ac);

    bool isBot() override {return true;};

protected:
    std::unique_ptr<GameState> tempGameState;
    double timeLimit;
public:

    BotPlayingReturn getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

    bool isTerminal();

    double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position);

    double virtual evaluate();

    bool exceedTimeLimit(sf::Clock &clock);

    bool isDraw();

};