#pragma once
#include <movable/Actor.hh>
#include <lightCycle/utility.hh>
#include <SFML/System.hpp>
#include <lightCycle/GameState.hh>
#include <set>
#include <algorithm>

class Bot5 : public Actor {
public:
    Bot5(Location l, Location mapSize, TileColor ac);

    bool isBot() override {return true;};

protected:
    std::unique_ptr<GameState> tempGameState;
    double timeLimit;

    double numMove = 0;
public:


    BotPlayingReturn getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

    bool isTerminal();

    double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta);

    double virtual evaluate();

    bool exceedTimeLimit(sf::Clock &clock);

    bool isDraw();

    static std::vector<std::vector<ComponentInfo>> VoronoiDiagram(GameState &game);
    static std::vector<Location> getNeighbours(GameState &game, Location l);

    static std::vector<std::vector<bool>> findArticulationPoint(GameState &game);
    static void findPoints(
        GameState &game, 
        Location u, 
        Location parent, 
        int &time, 
        std::vector<std::vector<int>> &disc, 
        std::vector<std::vector<int>> & low, 
        std::vector<std::vector<int>> &visited,
        std::vector<std::vector<bool>> &isAP
    );

    static std::set<Location> getArticulationPoints(GameState &game);

    static bool exploreSpace(
        GameState &game,
        std::vector<std::vector<ComponentInfo>> &voronoi, 
        std::vector<std::vector<bool>> &isAP,
        std::vector<std::vector<bool>> &visited,
        Location u,
        int &spaceCount,
        int owner,
        bool battleFrontCount
    );

    static double calculateTreeOfChamber(GameState &game, bool print);

    static bool areInSameComponent(GameState &game);
    static int floodFillAlgorithm(GameState &game, std::vector<std::vector<bool>> &visited, Location u);

    std::vector<Direction> moveOrdering(Position pos);

    BotPlayingReturn getMoveEndGame(GameState &game, sf::Clock &clock);

    double simpleEvaluate();

    double simpleMinimax(sf::Clock &clock, int depth, int &position);
};
