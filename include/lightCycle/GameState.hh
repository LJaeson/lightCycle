// #include <lightCycle/lightCycle.hh>
#pragma once
#include <lightCycle/Game.hh>
#include <stack>

struct Move {
    Position *pos;
    Direction dir;
};

class GameState {
    public:
        int w;
        int h;
        
        std::vector<std::vector<TileColor>> grid;
        Position bot;
        Position oponent;

        std::stack<Move> moveStack;

        GameState(int w, int h);
        GameState(Location l);

        void copyGame(const Game &game, TileColor botColor, TileColor opColor);
        
        bool getCrashed(Location l);

        std::vector<Direction> getMove(Position &pos);
        std::vector<Direction> getPossibleMove(Position &pos);

        void move(Position &pos, Direction dir);

        void unmove();
};