#include <lightCycle/lightCycle.hh>
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

        void copyGame(Game game, TileColor botColor, TileColor opColor);
        
        bool getCrashed(Location l);

        std::vector<Direction> getPossibleMove(Position &pos);

        void move(Position &pos, Direction dir);

        void unmove();
};