#include <movable/Bot.hh>

#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define MAX_DEPTH 20


Bot::Bot(Location l, TileColor ac)
    : Actor(l, ac),
    tempGameState(l) 
    {
        srand(time(0));
    }


// Bot::Bot(int w, int h): tempGame(w, h) {
//     srand(time(0));
// 

Direction Bot::getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
    sf::Clock clock;
    this->timeLimit = timeLimit;
    tempGameState.copyGame(game, color1, color2);
    
    int position = 0;
    Direction bestMove = Direction::DOWN;
    double maxEval = -std::numeric_limits<double>::infinity();
    for (Direction dir: tempGameState.getPossibleMove(tempGameState.bot)) {
        //std::cout << "get Move loop: " << dir << std::endl;
        if (exceedTimeLimit(clock)) {
            return bestMove;
        }
        tempGameState.move(tempGameState.bot, dir);
        double eval = minimax(MAX_DEPTH, false, clock, position);
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = dir;
        } else if (eval == maxEval && rand() % 2 == 1) {
            bestMove = dir;
        }
        tempGameState.unmove();
    }
    std::cout << position << std::endl;
    return bestMove;
}

bool Bot::botCanNotMove() {
    return tempGameState.getPossibleMove(tempGameState.bot).empty();

};

bool Bot::oponentCanNotMove() {
    return tempGameState.getPossibleMove(tempGameState.oponent).empty();
}

bool Bot::isTerminal() {
    return isDraw() || tempGameState.getCrashed(tempGameState.bot.location) || tempGameState.getCrashed(tempGameState.oponent.location);
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
        for (Direction dir: tempGameState.getPossibleMove(tempGameState.bot)) {
            tempGameState.move(tempGameState.bot, dir);
            maxEval = std::max(maxEval, minimax(depth - 1, false, clock, position));
            tempGameState.unmove();
        }
        return maxEval;

    } else {
        double minEval = std::numeric_limits<double>::infinity();
        for (Direction dir: tempGameState.getPossibleMove(tempGameState.oponent)) {
            tempGameState.move(tempGameState.oponent, dir);
            minEval = std::min(minEval, minimax(depth - 1, true, clock, position));
            tempGameState.unmove();
        }
        return minEval;
    }
}

bool Bot::isDraw() {
    return (tempGameState.getCrashed(tempGameState.bot.location) 
           && tempGameState.getCrashed(tempGameState.oponent.location)) 
           || tempGameState.bot.location.equal(tempGameState.oponent.location);
}

double Bot::evaluate() {
    if (isDraw()) {
        return -50; // draw
    } else if (tempGameState.getCrashed(tempGameState.bot.location)) {
        return -100; // oponent win
    } else if (tempGameState.getCrashed(tempGameState.oponent.location)) {
        return 100;
    }
    return 50;
    // return 5 * tempGameState.getPossibleMove(tempGameState.bot).size();
}