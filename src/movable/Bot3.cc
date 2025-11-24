#include <movable/Bot3.hh>
#include <limits.h>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <queue>

#define MAX_DEPTH 15


Bot3::Bot3(Location l, Location mapSize, TileColor ac)
    : Actor(l, ac),
    tempGameState(std::make_unique<GameState>(mapSize))
    {
        srand(time(0));
    }

BotPlayingReturn Bot3::getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
    
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

bool Bot3::isTerminal() {
    return isDraw() || tempGameState->getCrashed(tempGameState->bot.location) || tempGameState->getCrashed(tempGameState->oponent.location);
};

bool Bot3::exceedTimeLimit(sf::Clock &clock) {
    return clock.getElapsedTime().asMilliseconds() >= timeLimit;
}

double Bot3::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta) {
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

bool Bot3::isDraw() {
    return (tempGameState->getCrashed(tempGameState->bot.location) 
           && tempGameState->getCrashed(tempGameState->oponent.location)) 
           || tempGameState->bot.location.equal(tempGameState->oponent.location);
}

double Bot3::evaluate() {
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
    return (botTeritory - oppTeritory) * 10.0 + evaluateBoardCoverage() * 10.0;
}

std::vector<std::vector<ComponentInfo>> Bot3::VoronoiDiagram(GameState &game) {
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

std::vector<Location> Bot3::getNeighbours(GameState &game, Location l) {
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

double Bot3::evaluateBoardCoverage() {
    double score = 0.0;
    
    
    // get center of mass of all empty tiles
    int emptyCount = 0;
    double emptyCenterW = 0.0;
    double emptyCenterH = 0.0;
    
    for (int w = 0; w < tempGameState->w; w++) {
        for (int h = 0; h < tempGameState->h; h++) {
            Location loc = {w, h};
            if (!tempGameState->getCrashed(loc)) {
                emptyCenterW += w;
                emptyCenterH += h;
                emptyCount++;
            }
        }
    }
    
    if (emptyCount > 0) {
        emptyCenterW /= emptyCount;
        emptyCenterH /= emptyCount;
    }
    
    // find how far each player is from the empty space center
    double botDistToEmpty = std::abs(tempGameState->bot.location.w - emptyCenterW) + 
                           std::abs(tempGameState->bot.location.h - emptyCenterH);
    double opDistToEmpty = std::abs(tempGameState->oponent.location.w - emptyCenterW) + 
                          std::abs(tempGameState->oponent.location.h - emptyCenterH);
    
    // reward for being closer to empty space
    score += (opDistToEmpty - botDistToEmpty) * 0.5;
    
    // === QUADRANT BASED SEPARATION ===
    int centerW = tempGameState->w / 2;
    int centerH = tempGameState->h / 2;
    
    bool botLeft = tempGameState->bot.location.w < centerW;
    bool botTop = tempGameState->bot.location.h < centerH;
    bool opLeft = tempGameState->oponent.location.w < centerW;
    bool opTop = tempGameState->oponent.location.h < centerH;
    
    // big penalty for being in same quadrant - forces separation
    if (botLeft == opLeft && botTop == opTop) {
        score -= 50.0; 
    }
    // penalise for same horizontal or vertical half
    else if (botLeft == opLeft || botTop == opTop) {
        score -= 20.0;  
    }
    
    // reawrd for diagonal quadrants (best separation)
    if (botLeft != opLeft && botTop != opTop) {
        score += 25.0;
    }
    
    return score;
}