#pragma once
#include <movable/Bot.hh>
#include <vector>

class Bot2 : public Bot {
    public:
        Bot2(Location l, Location mapSize, TileColor ac): Bot(l, mapSize, ac) {};
        
        double virtual evaluate() override;

        static std::vector<std::vector<char>> VoronoiDiagram(GameState game);
};