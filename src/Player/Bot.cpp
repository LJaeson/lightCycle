#include <Player/Bot.hpp>
#include <limits>
#include <algorithm>

#define MAX_DEPTH 20

Bot::Bot(int w, int h): game(w, h) {}

bool Bot::clientControlled() {
    return false;
}

Direction Bot::getMove(Game gameState, TileColor color1, TileColor color2, double timeLimit) {
    sf::Clock clock;
    this->timeLimit = timeLimit;
    game.copyGame(gameState, color1, color2);
    
    int position = 0;
    Direction bestMove = Direction::DOWN;
    double maxEval = -std::numeric_limits<double>::infinity();
    for (Direction dir: game.getPossibleMove(game.bot)) {
        // std::cout << "get Move loop: " << dir << std::endl;
        if (exceedTimeLimit(clock)) {
            return bestMove;
        }
        game.move(game.bot, dir);
        double eval = minimax(MAX_DEPTH, false, clock, position);
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = dir;
        }
        game.unmove();
    }
    
    return bestMove;
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

bool Bot::exceedTimeLimit(sf::Clock &clock) {
    return clock.getElapsedTime().asMilliseconds() >= timeLimit;
}

double Bot::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position) {
    if (isTerminal() || depth == 0 || exceedTimeLimit(clock)) {
        return evaluate();
    }
    ++position;
    if (maximizingPlayer) {
        double maxEval = -std::numeric_limits<double>::infinity();
        for (Direction dir: game.getPossibleMove(game.bot)) {
            game.move(game.bot, dir);
            maxEval = std::max(maxEval, minimax(depth - 1, false, clock, position));
            game.unmove();
        }
        return maxEval;

    } else {
        double minEval = std::numeric_limits<double>::infinity();
        for (Direction dir: game.getPossibleMove(game.oponent)) {
            game.move(game.oponent, dir);
            minEval = std::min(minEval, minimax(depth - 1, true, clock, position));
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

    return 5 * game.getPossibleMove(game.bot).size();
}