#pragma once
#include <movable/Actor.hh>
#include <lightCycle/utility.hh>
#include <SFML/System.hpp>
#include <lightCycle/GameState.hh>

class Bot3 : public Actor {
public:
    Bot3(Location l, Location mapSize, TileColor ac);

    bool isBot() override {return true;};

protected:
    std::unique_ptr<GameState> tempGameState;
    double timeLimit;
public:


    BotPlayingReturn getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

    bool isTerminal();

    double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta);

    double virtual evaluate();

    bool exceedTimeLimit(sf::Clock &clock);

    bool isDraw();

    static std::vector<std::vector<ComponentInfo>> VoronoiDiagram(GameState &game);
    static std::vector<Location> getNeighbours(GameState &game, Location l);

    double evaluateBoardCoverage();
};