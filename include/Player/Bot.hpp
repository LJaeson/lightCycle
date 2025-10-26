#include <Player/Player.hpp>
#include <Player/GameState.hpp>

class Bot: public Player {
    private:
        GameState game;
        double timeLimit;
    public:
        Bot(int w, int h);

        bool clientControlled() override;

        Direction getMove(Game game, TileColor color1, TileColor color2, double timeLimit) override;

        bool botCanNotMove();
        bool oponentCanNotMove();

        bool isTerminal();

        double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position);

        double evaluate();

        bool exceedTimeLimit(sf::Clock &clock);
};