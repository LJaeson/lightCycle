#include <movable/Bot2.hh>
#include <limits.h>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <queue>

#define MAX_DEPTH 10


Bot2::Bot2(Location l, Location mapSize, TileColor ac)
    : Actor(l, ac),
    tempGameState(std::make_unique<GameState>(mapSize))
    {
        srand(time(0));
    }

BotPlayingReturn Bot2::getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
    
    sf::Clock clock;
    this->timeLimit = timeLimit;
    tempGameState->copyGame(game, color1, color2);

    if (tempGameState->bot.location.equal({-1, -1}) || tempGameState->oponent.location.equal({-1, -1})) return {Direction::DOWN, 0, clock.getElapsedTime().asMilliseconds()};
    
    int position = 0;
    Direction bestMove = Direction::DOWN;

    // Set the basic value
    double maxEval = -std::numeric_limits<double>::infinity();
    double alpha = -std::numeric_limits<double>::infinity();
    double beta = std::numeric_limits<double>::infinity();

    for (Direction dir: tempGameState->getMove(tempGameState->bot)) {
        if (exceedTimeLimit(clock)) {
            return {bestMove, position, clock.getElapsedTime().asMilliseconds()};
        }
        tempGameState->move(tempGameState->bot, dir);
        double eval = minimax(MAX_DEPTH, false, clock, position, alpha, beta);
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = dir;
        } else if (eval == maxEval && rand() % 2 == 1) {
            bestMove = dir;
        }
        tempGameState->unmove();
    }
    return {bestMove, position, clock.getElapsedTime().asMilliseconds()};
}

bool Bot2::isTerminal() {
    return isDraw() || tempGameState->getCrashed(tempGameState->bot.location) || tempGameState->getCrashed(tempGameState->oponent.location);
};

bool Bot2::exceedTimeLimit(sf::Clock &clock) {
    return clock.getElapsedTime().asMilliseconds() >= timeLimit;
}

double Bot2::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta) {
    if (isTerminal() || depth == 0 || exceedTimeLimit(clock)) {
        return evaluate();
    }
    ++position;
    if (maximizingPlayer) {
        double maxEval = -std::numeric_limits<double>::infinity();
        for (Direction dir: tempGameState->getMove(tempGameState->bot)) {
            tempGameState->move(tempGameState->bot, dir);
            maxEval = std::max(maxEval, minimax(depth - 1, false, clock, position, alpha, beta));
            tempGameState->unmove();
            alpha = std::max(alpha, maxEval);
            if (beta <= alpha) {
                break; // beta cutoff - opp won't allow this path
            }
        }
        return maxEval;

    } else {
        double minEval = std::numeric_limits<double>::infinity();
        for (Direction dir: tempGameState->getMove(tempGameState->oponent)) {
            tempGameState->move(tempGameState->oponent, dir);
            minEval = std::min(minEval, minimax(depth - 1, true, clock, position, alpha, beta));
            tempGameState->unmove();
            beta = std::min(beta, minEval);
            if (beta <= alpha) {
                break; // alpha cutoff - we won't choose this path
            }
        }
        return minEval;
    }
}

bool Bot2::isDraw() {
    return (tempGameState->getCrashed(tempGameState->bot.location) 
           && tempGameState->getCrashed(tempGameState->oponent.location)) 
           || tempGameState->bot.location.equal(tempGameState->oponent.location);
}

double Bot2::evaluate() {
    if (isDraw()) {
        return -500; // draw
    } else if (tempGameState->getCrashed(tempGameState->bot.location)) {
        return -1000; // oponent win
    } else if (tempGameState->getCrashed(tempGameState->oponent.location)) {
        return 1000; // the bot (us) win
    }
    
    std::vector<std::vector<ComponentInfo>> voronoi = VoronoiDiagram(*tempGameState);
    double botTeritory = 0;
    double oppTeritory = 0;
    for (int i = 0; i < tempGameState->w; ++i) {
        for (int j = 0; j < tempGameState->h; ++j) {
            if (voronoi[i][j].owner == BOT_OWNER) {
                ++botTeritory;
            } else if (voronoi[i][j].owner == OPP_OWNER) {
                ++oppTeritory;
            }
        }
    }
    return (botTeritory - oppTeritory) * 10;
}

std::vector<std::vector<ComponentInfo>> Bot2::VoronoiDiagram(GameState &game) {
    int w = game.w;
    int h = game.h;

    std::vector<std::vector<ComponentInfo>> voronoi(w, std::vector<ComponentInfo>(h, {-1, UNVISIT}));

    std::queue<Location> bfsQueue;

    voronoi[game.bot.location.w][game.bot.location.h] = { 0, BOT_OWNER};
    voronoi[game.oponent.location.w][game.oponent.location.h] = { 0, OPP_OWNER };

    bfsQueue.push(game.bot.location);
    bfsQueue.push(game.oponent.location);

    while (!bfsQueue.empty()) {
        Location loc = bfsQueue.front();
        int owner = voronoi[loc.w][loc.h].owner;
        bfsQueue.pop();

        for (Location neighbour : getNeighbours(game, loc)) {
            int newDist = voronoi[loc.w][loc.h].distToNearestPlayer + 1;
            int curr = voronoi[neighbour.w][neighbour.h].owner;

            if (curr == UNVISIT) {
                voronoi[neighbour.w][neighbour.h] = { newDist, owner };
                bfsQueue.push(neighbour);
            } else if (
                voronoi[neighbour.w][neighbour.h].distToNearestPlayer == newDist && 
                voronoi[neighbour.w][neighbour.h].owner != owner
            ) {
                voronoi[neighbour.w][neighbour.h] = { newDist, EQUAL_OWNER };
            }
        }
    }

    return voronoi;
}

std::vector<Location> Bot2::getNeighbours(GameState &game, Location l) {
    std::vector<Location> neighbour;
    int dw[] = {0, 1, 0, -1};
    int dh[] = {-1, 0, 1, 0};

    for (int dir = 0; dir < 4; ++dir) {
        Location next = {l.w + dw[dir], l.h + dh[dir]};
        if (!game.getCrashed(next)) {
            neighbour.push_back(next);
        }
    }
    return neighbour;
}