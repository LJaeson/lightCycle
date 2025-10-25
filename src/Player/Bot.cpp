#include <Player/Bot.hpp>
#include <limits>
#include <algorithm>

Bot::Bot(int w, int h): game(w, h) {}

bool Bot::clientControlled() {
    return false;
}

Direction Bot::getMove(Game game, TileColor color1, TileColor color2, double timeLimit) {
    sf::Clock clock;
    this->timeLimit = timeLimit;
    this->game.copyGame(game, color1, color2);

    
}

bool Bot::botCanNotMove() {
    return game.getPossibleMove(game.bot).empty();
};

bool Bot::oponentCanNotMove() {
    return game.getPossibleMove(game.oponent).empty();
}

bool Bot::isTerminal() {
    return botCanNotMove() || oponentCanNotMove();
};

double Bot::minimax(int depth, bool maximizingPlayer, sf::Clock &clock) {
    if (isTerminal() || depth == 0 || clock.getElapsedTime().asMilliseconds() >= timeLimit) {
        return evaluate();
    }

    if (maximizingPlayer) {
        double maxEval = std::numeric_limits<double>::infinity();
        for (Direction dir: game.getPossibleMove(game.bot)) {
            game.move(game.bot, dir);
            maxEval = std::max(maxEval, minimax(depth - 1, false, clock));
            game.unmove();
        }
        return maxEval;

    } else {
        double minEval = -std::numeric_limits<double>::infinity();
        for (Direction dir: game.getPossibleMove(game.oponent)) {
            game.move(game.oponent, dir);
            minEval = std::min(minEval, minimax(depth - 1, true, clock));
            game.unmove();
        }
        return minEval;
    }
}

double Bot::evaluate() {
    if (oponentCanNotMove() && botCanNotMove()) {
        return -50; // draw
    } else if (botCanNotMove()) {
        return -100; // oponent win
    } else if (oponentCanNotMove()) {
        return 100;
    }

    
}