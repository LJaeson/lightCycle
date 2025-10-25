#include <Player/Player.hpp>
#include <Player/GameState.hpp>

class Bot: public Player {
    private:
        GameState game;
        double timeLimit;
    public:
        Bot(int w, int h);

        virtual bool clientControlled() override;

        virtual Direction getMove(Game game, TileColor color1, TileColor color2, double timeLimit) override;

        bool botCanNotMove();
        bool oponentCanNotMove();
        
        bool isTerminal();

        double minimax(int depth, bool maximizingPlayer, sf::Clock &clock);

        double evaluate();
};