#include <movable/Bot5.hh>
#include <limits.h>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <queue>

#define MAX_DEPTH 7

Bot5::Bot5(Location l, Location mapSize, TileColor ac)
    : Actor(l, ac),
    tempGameState(std::make_unique<GameState>(mapSize))
    {
        srand(time(0));
    }

BotPlayingReturn Bot5::getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
    ++numMove;
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

    for (Direction dir: moveOrdering(tempGameState->bot)) {
        if (tempGameState->getCrashed(tempGameState->bot.findNextLocationFromDirection(dir))) {
            continue;
        }
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

std::vector<Direction> Bot5::moveOrdering(Position pos) {

    int w = tempGameState->w;
    int h = tempGameState->h;

    // std::vector<std::vector<bool>> isAP = findArticulationPoint(*tempGameState);

    std::vector<std::pair<Direction, double>> moves;

    for (Direction dir : tempGameState->getMove(pos)) {
        Location newL = pos.findNextLocationFromDirection(dir);
        double score = 0;

        if (tempGameState->getCrashed(newL)) {
            score -= 1000;
            moves.push_back({dir, score});
            continue;
        }

        tempGameState->move(pos, dir);
        
        // auto futureAp = findArticulationPoint(*tempGameState);

        // if (isAP[newL.w][newL.h]) score-= 50;

        // Free space left to move
        score -= getNeighbours(*tempGameState, newL).size() * 5;

        // Check if next move cause the next next move to be AP
        // for (Location l : getNeighbours(*tempGameState, newL)) {
        //     if (futureAp[l.w][l.h]) score -= 20;
        // }
    
        // Manhattan dist to the center
        Location center = { w/2, h/2 };
        int dist = std::abs(newL.w - center.w) + std::abs(newL.h - center.h);
        double decayFactor = 0.80;
        double f = 0.4;
        for (int i = 0; i < numMove; ++i) {
            f *= decayFactor;
        }
        score -= dist * f;

        // Flood fill space of the new location
        // std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, 0));
        // int newSpace = floodFillAlgorithm(*tempGameState, visited, newL);
        // score += newSpace * 0.1;
    
        // if (areInSameComponent(*tempGameState)) {
            
        //     if (!areInSameComponent(*tempGameState)) {
        //         score += 10;

        //         std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, 0));

        //         int botSpace = floodFillAlgorithm(*tempGameState, visited, tempGameState->bot.location);
        //         int oppSpace = floodFillAlgorithm(*tempGameState, visited, tempGameState->oponent.location);

        //         if (botSpace > oppSpace) {
        //             score += 300;
        //         }
        //     }
        // }

        tempGameState->unmove();

        moves.push_back({dir, score});
    }

    std::sort(moves.begin(), moves.end(), 
              [](const auto &a, const auto &b) { return a.second > b.second; });

    std::vector<Direction> orderMove;
    for (const auto &a : moves) {
        orderMove.push_back(a.first);
    }
    return orderMove;
}

bool Bot5::isTerminal() {
    return isDraw() || tempGameState->getCrashed(tempGameState->bot.location) || tempGameState->getCrashed(tempGameState->oponent.location);
};

bool Bot5::exceedTimeLimit(sf::Clock &clock) {
    return clock.getElapsedTime().asMilliseconds() >= timeLimit;
}

double Bot5::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta) {
    if (isTerminal() || depth == 0 || exceedTimeLimit(clock)) {
        return evaluate();
    }
    ++position;
    if (maximizingPlayer) {
        double maxEval = -std::numeric_limits<double>::infinity();
        for (Direction dir: moveOrdering(tempGameState->bot)) {
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
        for (Direction dir: moveOrdering(tempGameState->oponent)) {
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

bool Bot5::isDraw() {
    return (tempGameState->getCrashed(tempGameState->bot.location) 
           && tempGameState->getCrashed(tempGameState->oponent.location)) 
           || tempGameState->bot.location.equal(tempGameState->oponent.location);
}

double Bot5::evaluate() {
    if (isDraw()) {
        return -500; // draw
    } else if (tempGameState->getCrashed(tempGameState->bot.location)) {
        return -100000; // oponent win
    } else if (tempGameState->getCrashed(tempGameState->oponent.location)) {
        return 100000; // the bot (us) win
    }
    
    // if (!areInSameComponent(*tempGameState)) {
    //     int w = tempGameState->w;
    //     int h = tempGameState->h;

        
    //     std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, 0));

    //     int botSpace = floodFillAlgorithm(*tempGameState, visited, tempGameState->bot.location);
    //     int oppSpace = floodFillAlgorithm(*tempGameState, visited, tempGameState->oponent.location);

    //     double spaceScore = (botSpace - oppSpace) * 5;
    //     if (botSpace > oppSpace) {
    //         spaceScore += 300;
    //     }
    //     return spaceScore;
    // }
    double eval = calculateTreeOfChamber(*tempGameState, false) * 100;

    if (!areInSameComponent(*tempGameState)) {
        eval -= 100;
        if (eval > 0) {
            eval += 300;
        }
    }

    return eval;
}

std::vector<std::vector<ComponentInfo>> Bot5::VoronoiDiagram(GameState &game) {
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

std::vector<Location> Bot5::getNeighbours(GameState &game, Location l) {
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

double Bot5::calculateTreeOfChamber(GameState &game, bool print) {
    int w = game.w;
    int h = game.h;

    std::vector<std::vector<ComponentInfo>> voronoi = VoronoiDiagram(game);
    std::vector<std::vector<bool>> isAP = findArticulationPoint(game);

    std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, 0));
    
    // List of all the articulation points
    std::vector<Location> AP;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            if (isAP[i][j]) {
                AP.push_back({i, j});
            }
        }
    }

    int botSpace = 0;
    exploreSpace(game, voronoi, isAP, visited, game.bot.location, botSpace, BOT_OWNER, true);

    int oppSpace = 0;
    exploreSpace(game, voronoi, isAP, visited, game.oponent.location, oppSpace, OPP_OWNER, true);

    int maxBotAPChamber = 0;
    int maxOppAPChamber = 0;
    for (Location ap : AP) {
        int owner = voronoi[ap.w][ap.h].owner;

        if (owner == EQUAL_OWNER || owner == UNVISIT) {
            continue;
        }

        std::vector<std::vector<bool>> apVisited = visited;
        int space = 0;
        exploreSpace(game, voronoi, isAP, apVisited, ap, space, owner, true);

        if (owner == BOT_OWNER) {
            maxBotAPChamber = std::max(maxBotAPChamber, space);
        } else if (owner == OPP_OWNER) {
            maxOppAPChamber = std::max(maxOppAPChamber, space);
        }
    }

    if (print) {
        double botTeritory = 0;
        double oppTeritory = 0;
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                if (voronoi[i][j].owner == BOT_OWNER) {
                    ++botTeritory;
                } else if (voronoi[i][j].owner == OPP_OWNER) {
                    ++oppTeritory;
                }
            }
        }

        std::cout << "Voronoi Player 1 Space: " << botTeritory << ", Tree of Chamber Player 1 Space: " << botSpace << ", AP Chamber: " << maxBotAPChamber << std::endl;
        std::cout << "Voronoi Player 2 Space: " << oppTeritory << ", Tree of Chamber Player 2 Space: " << oppSpace << ", AP Chamber: " << maxOppAPChamber << std::endl;
        std::cout << "======================\n";
    }

    return (botSpace + maxBotAPChamber) - (oppSpace + maxOppAPChamber);
 }

bool Bot5::exploreSpace(
    GameState &game,
    std::vector<std::vector<ComponentInfo>> &voronoi, 
    std::vector<std::vector<bool>> &isAP,
    std::vector<std::vector<bool>> &visited,
    Location u,
    int &spaceCount,
    int owner,
    bool battleFrontCount
) {
    visited[u.w][u.h] = true;
    if (voronoi[u.w][u.h].owner == owner && !isAP[u.w][u.h]) {
        ++spaceCount;
    }
    for (Location v : getNeighbours(game, u)) {
        if (!visited[v.w][v.h]) {
            if (voronoi[v.w][v.h].owner == EQUAL_OWNER && !battleFrontCount) {
                spaceCount = 0;
                return false;
            }
            if (voronoi[v.w][v.h].owner == owner && !isAP[v.w][v.h]) {
                bool keep = exploreSpace(game, voronoi, isAP, visited, v, spaceCount, owner, battleFrontCount);
                if (!keep) {
                    spaceCount = 0;
                    return false;
                }
            }
        }
    }
    return true;
}

std::vector<std::vector<bool>> Bot5::findArticulationPoint(GameState &game) {
    int w = game.w;
    int h = game.h;

    int time = 0;
    std::vector<std::vector<int>> disc(w, std::vector<int>(h, 0)),
                                  low(w, std::vector<int>(h, 0)),
                                  visited(w, std::vector<int>(h, 0));

    std::vector<std::vector<bool>> isAP(w, std::vector<bool>(h, 0));

    findPoints(game, game.bot.location, {-1, -1}, time, disc, low, visited, isAP);
    findPoints(game, game.oponent.location, {-1, -1}, time, disc, low, visited, isAP);

    return isAP;
}

void Bot5::findPoints(
    GameState &game, 
    Location u, 
    Location parent, 
    int &time, 
    std::vector<std::vector<int>> &disc, 
    std::vector<std::vector<int>> & low, 
    std::vector<std::vector<int>> &visited, 
    std::vector<std::vector<bool>> &isAP
) {
    visited[u.w][u.h] = 1;
    disc[u.w][u.h] = low[u.w][u.h] = ++time;

    int dfsChildren = 0;

    for (Location v : getNeighbours(game, u)) {
        if (!visited[v.w][v.h]) {
            ++dfsChildren;

            findPoints(game, v, u, time, disc, low, visited, isAP);

            low[u.w][u.h] = std::min(low[u.w][u.h], low[v.w][v.h]);

            // v can not reach ancestor of u, thus, removal of u will 
            if (!parent.equal({-1, -1}) && low[v.w][v.h] >= disc[u.w][u.h]) {
                isAP[u.w][u.h] = 1;
            }
        } else if (!v.equal(parent)) {
            low[u.w][u.h] = std::min(low[u.w][u.h], disc[v.w][v.h]);
        }
    }

    if (parent.equal({-1, -1}) && dfsChildren > 1) {
        isAP[u.w][u.h] = 1;
    }
}

std::set<Location> Bot5::getArticulationPoints(GameState &game) {
    std::vector<std::vector<bool>> isAP = findArticulationPoint(game);

    std::set<Location> AP;
    for (int i = 0; i < game.w; i++) {
        for (int j = 0; j < game.h; j++) {
            if (isAP[i][j]) {
                AP.insert({i, j});
            }
        }
    }

    return AP;
}

bool Bot5::areInSameComponent(GameState &game) {
    int w = game.w;
    int h = game.h;

    std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, 0));

    floodFillAlgorithm(game, visited, game.bot.location);

    if (visited[game.oponent.location.w][game.oponent.location.h]) {
        return true;
    }
    return false;
}

int Bot5::floodFillAlgorithm(GameState &game, std::vector<std::vector<bool>> &visited, Location u)  {
    visited[u.w][u.h] = 1;

    int space = 1;
    for (Location v : getNeighbours(game, u)) {
        if (visited[v.w][v.h]) {
            continue;
        }
        space += floodFillAlgorithm(game, visited, v);
    }

    return space;
}