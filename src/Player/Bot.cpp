#include <Player/Bot.hpp>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <queue>
#include <set>
#include <cmath>

// reduced from 10 to 6 for better performance (256x fewer nodes at max depth)
#define MAX_DEPTH 6

Bot::Bot(int w, int h): game(w, h) {
    srand(time(0));
}

bool Bot::clientControlled() {
    return false;
}

Direction Bot::getMove(const Game &gameState, TileColor color1, TileColor color2, double timeLimit) {
    sf::Clock clock;
    this->timeLimit = timeLimit;
    game.copyGame(gameState, color1, color2);
    
    int position = 0;
    Direction bestMove = Direction::DOWN;
    double maxEval = -std::numeric_limits<double>::infinity();
    for (Direction dir: game.getPossibleMove(game.bot)) {
        //std::cout << "get Move loop: " << dir << std::endl;
        if (exceedTimeLimit(clock)) {
            return bestMove;
        }
        game.move(game.bot, dir);
        double eval = minimax(MAX_DEPTH, false, clock, position);
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = dir;
        } else if (eval == maxEval && rand() % 2 == 1) {
            bestMove = dir;
        }
        game.unmove();
    }
    std::cout << position << std::endl;
    return bestMove;
}

bool Bot::botCanNotMove() {
    return game.getPossibleMove(game.bot).empty();

};

bool Bot::oponentCanNotMove() {
    return game.getPossibleMove(game.oponent).empty();
}

bool Bot::isTerminal() {
    return isDraw() || game.getCrashed(game.bot.location) || game.getCrashed(game.oponent.location);
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

bool Bot::isDraw() {
    return (game.getCrashed(game.bot.location) 
           && game.getCrashed(game.oponent.location)) 
           || game.bot.location.equal(game.oponent.location);
}

double Bot::evaluate() {
    if (isDraw()) {
        return -50; // draw
    } else if (game.getCrashed(game.bot.location)) {
        return -100; // oponent win
    } else if (game.getCrashed(game.oponent.location)) {
        return 100;
    }
    
    // changed from simple heuristic to survival mode evaluation
    return evaluateSurvival();
}

// ==================== SURVIVAL MODE IMPLEMENTATION ====================

// some current problems: bot can be little dumb, it traps itself, 
// and towards endgame the bot goes out of bounds 

Bot::GameStage Bot::determineGameStage() {
    // count total occupied squares
    int totalOccupied = 0;
    int totalSquares = game.w * game.h;
    
    for (int i = 0; i < game.w; ++i) {
        for (int j = 0; j < game.h; ++j) {
            if (game.grid[i][j] == TileColor::BOUNDARY) {
                totalOccupied++;
            }
        }
    }
    
    double fillRatio = (double)totalOccupied / totalSquares;
    
    // early game: less than 30% filled
    if (fillRatio < 0.3) {
        return EARLY_GAME;
    }
    // end game: more than 70% filled or players in different components
    else if (fillRatio > 0.7 || arePlayersInDifferentComponents()) {
        return END_GAME;
    }
    // mid game: between 30% and 70%
    else {
        return MID_GAME;
    }
}

double Bot::evaluateSurvival() {
    GameStage stage = determineGameStage();
    double score = 0.0;
    
    // reward staying away from opponent
    double distBonus = distanceToOpponent() * 2.0;
    score += distBonus;
    
    // evaluate based on stage
    switch (stage) {
        case EARLY_GAME:
            score += evaluateVoronoi() * 3.0;
            break;
        case MID_GAME:
            score += evaluateArticulationPoints() * 2.5;
            break;
        case END_GAME:
            score += evaluateLongestPath() * 4.0;
            break;
    }
    
    // bonus for having more available space
    int botSpace = floodFillCount(game.bot.location);
    int opSpace = floodFillCount(game.oponent.location);
    score += (botSpace - opSpace) * 1.5;
    
    return score;
}

// early game: voronoi-based territory evaluation using multi-source bfs
// simultaneous bfs from both players to partition board into territories
double Bot::evaluateVoronoi() {
    std::set<std::pair<int,int>> visited;
    std::vector<std::vector<int>> distances(game.w, std::vector<int>(game.h, -1));
    
    // bfs from both players to calculate voronoi regions
    std::queue<std::pair<Location, int>> bfsQueue;
    std::vector<std::vector<int>> owner(game.w, std::vector<int>(game.h, -1)); // 0=bot, 1=oponent, -1=unowned
    
    // Start bfs from both players simultaneously
    bfsQueue.push({game.bot.location, 0});
    bfsQueue.push({game.oponent.location, 1});
    distances[game.bot.location.w][game.bot.location.h] = 0;
    distances[game.oponent.location.w][game.oponent.location.h] = 0;
    owner[game.bot.location.w][game.bot.location.h] = 0;
    owner[game.oponent.location.w][game.oponent.location.h] = 1;
    
    int botTerritory = 0;
    int opTerritory = 0;
    int maxIterations = 150; // made it small to avoid lagging 
    int iterations = 0;
    
    while (!bfsQueue.empty() && iterations < maxIterations) {
        auto [loc, playerID] = bfsQueue.front();
        bfsQueue.pop();
        iterations++;
        
        for (Location neighbor : getNeighbors(loc)) {
            if (isValidLocation(neighbor) && !game.getCrashed(neighbor)) {
                int newDist = distances[loc.w][loc.h] + 1;
                
                // if unvisited/equidistant from both players
                if (distances[neighbor.w][neighbor.h] == -1) {
                    distances[neighbor.w][neighbor.h] = newDist;
                    owner[neighbor.w][neighbor.h] = playerID;
                    bfsQueue.push({neighbor, playerID});
                    
                    if (playerID == 0) botTerritory++;
                    else opTerritory++;
                } else if (distances[neighbor.w][neighbor.h] == newDist && owner[neighbor.w][neighbor.h] != playerID) {
                    // equidistant - contested territory, don't count for anyone
                    owner[neighbor.w][neighbor.h] = -1;
                    if (playerID == 0 && botTerritory > 0) botTerritory--;
                    else if (playerID == 1 && opTerritory > 0) opTerritory--;
                }
            }
        }
    }
    
    return (double)(botTerritory - opTerritory);
}

// mid game: articulation point and connectivity evaluation
// detect chokepoints and reward high connectivity positions
// only checks immediate position and neighbours (was checking 5x5 radius)
double Bot::evaluateArticulationPoints() {
    double score = 0.0;
    
    // check only immediate pos instead of finding all articulation points
    if (isArticulationPoint(game.bot.location)) {
        score -= 20.0; // avoid narrow spaces
    }
    
    // reward positions with high connectivity
    int reachableSquares = countReachableSquares(game.bot.location, std::set<std::pair<int,int>>());
    score += reachableSquares * 0.5;
    
    // check immediate neighbors 
    for (Location neighbor : getNeighbors(game.bot.location)) {
        if (isValidLocation(neighbor) && !game.getCrashed(neighbor)) {
            if (isArticulationPoint(neighbor)) {
                score -= 10.0; // penalise being next to choke points
            }
        }
    }
    
    return score;
}

// end game: longest path evaluation for space filling
// linear flood fill for performance
double Bot::evaluateLongestPath() {
    // use a limited depth bfs
    int reachableSquares = floodFillCount(game.bot.location);
    
    // reward based on reachable space
    return reachableSquares * 2.0;
}

// check if players are in different connected components
// bfs from bot position to check if opponent is reachable
bool Bot::arePlayersInDifferentComponents() {
    std::set<std::pair<int,int>> visited;
    std::queue<Location> q;
    
    q.push(game.bot.location);
    visited.insert({game.bot.location.w, game.bot.location.h});
    
    while (!q.empty()) {
        Location current = q.front();
        q.pop();
        
        // if opponent reached, they're in the same component
        if (current.equal(game.oponent.location)) {
            return false;
        }
        
        for (Location neighbor : getNeighbors(current)) {
            if (isValidLocation(neighbor) && !game.getCrashed(neighbor)) {
                auto key = std::make_pair(neighbor.w, neighbor.h);
                if (visited.find(key) == visited.end()) {
                    visited.insert(key);
                    q.push(neighbor);
                }
            }
        }
    }
    
    return true; // opponent not reachable
}

// flood fill count from a location
// bfs to count all reachable empty squares
// limited to 200 iter. to prevent lag
int Bot::floodFillCount(Location start) {
    std::set<std::pair<int,int>> visited;
    std::queue<Location> q;
    
    q.push(start);
    visited.insert({start.w, start.h});
    int count = 0;
    int maxCount = 200; 
    
    while (!q.empty() && count < maxCount) {
        Location current = q.front();
        q.pop();
        count++;
        
        for (Location neighbor : getNeighbors(current)) {
            if (isValidLocation(neighbor) && !game.getCrashed(neighbor)) {
                auto key = std::make_pair(neighbor.w, neighbor.h);
                if (visited.find(key) == visited.end()) {
                    visited.insert(key);
                    q.push(neighbor);
                }
            }
        }
    }
    
    return count;
}

// get Manhattan distance to opponent
double Bot::distanceToOpponent() {
    int dx = abs(game.bot.location.w - game.oponent.location.w);
    int dy = abs(game.bot.location.h - game.oponent.location.h);
    return (double)(dx + dy);
}

// check if location is valid
bool Bot::isValidLocation(Location loc) {
    return loc.w >= 0 && loc.w < game.w && loc.h >= 0 && loc.h < game.h;
}

// get valid neighbours
std::vector<Location> Bot::getNeighbors(Location loc) {
    std::vector<Location> neighbors;
    int dw[] = {0, 1, 0, -1};
    int dh[] = {-1, 0, 1, 0};
    
    for (int i = 0; i < 4; ++i) {
        Location neighbor = {loc.w + dw[i], loc.h + dh[i]};
        if (isValidLocation(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

// count reachable squares from a location (excluding certain cells)
// limit to 100 iter for performance
int Bot::countReachableSquares(Location start, const std::set<std::pair<int,int>> &exclude) {
    std::set<std::pair<int,int>> visited;
    std::queue<Location> q;
    
    q.push(start);
    visited.insert({start.w, start.h});
    int count = 0;
    int maxCount = 100; 
    
    while (!q.empty() && count < maxCount) {
        Location current = q.front();
        q.pop();
        count++;
        
        for (Location neighbor : getNeighbors(current)) {
            auto key = std::make_pair(neighbor.w, neighbor.h);
            if (isValidLocation(neighbor) && !game.getCrashed(neighbor) && 
                visited.find(key) == visited.end() && exclude.find(key) == exclude.end()) {
                visited.insert(key);
                q.push(neighbor);
            }
        }
    }
    
    return count;
}

// check if a location is an articulation point 
// temporarily remove node and count resulting connected components
// has max search size and early exit for performance
bool Bot::isArticulationPoint(Location loc) {
    if (game.getCrashed(loc)) return false;
    
    // temporarily mark location as blocked
    TileColor originalColor = game.grid[loc.w][loc.h];
    game.grid[loc.w][loc.h] = TileColor::BOUNDARY;
    
    // count components in neighbours, limit search depth for performance
    std::set<std::pair<int,int>> globalVisited;
    int componentCount = 0;
    int maxSearchSize = 50; 
    
    for (Location neighbor : getNeighbors(loc)) {
        auto key = std::make_pair(neighbor.w, neighbor.h);
        if (isValidLocation(neighbor) && !game.getCrashed(neighbor) && 
            globalVisited.find(key) == globalVisited.end()) {
            
            // bfs to mark component
            std::queue<Location> q;
            q.push(neighbor);
            globalVisited.insert(key);
            int searchCount = 0;
            
            while (!q.empty() && searchCount < maxSearchSize) {
                Location current = q.front();
                q.pop();
                searchCount++;
                
                for (Location nextNeighbor : getNeighbors(current)) {
                    auto nextKey = std::make_pair(nextNeighbor.w, nextNeighbor.h);
                    if (isValidLocation(nextNeighbor) && !game.getCrashed(nextNeighbor) && 
                        globalVisited.find(nextKey) == globalVisited.end()) {
                        globalVisited.insert(nextKey);
                        q.push(nextNeighbor);
                    }
                }
            }
            
            componentCount++;
            if (componentCount > 1) {
                // early exit bcs we already know it's an articulation point
                game.grid[loc.w][loc.h] = originalColor;
                return true;
            }
        }
    }
    
    // restore original state
    game.grid[loc.w][loc.h] = originalColor;
    
    // if removing this point creates more than 1 component, it's an articulation point
    return componentCount > 1;
}

// find articulation points via DFS
void Bot::findArticulationPoints(Location start, std::set<Location> &articulationPoints) {
    int checkRadius = 5;
    
    for (int w = std::max(0, game.bot.location.w - checkRadius); 
         w < std::min(game.w, game.bot.location.w + checkRadius); ++w) {
        for (int h = std::max(0, game.bot.location.h - checkRadius); 
             h < std::min(game.h, game.bot.location.h + checkRadius); ++h) {
            Location loc = {w, h};
            if (!game.getCrashed(loc) && isArticulationPoint(loc)) {
                articulationPoints.insert(loc);
            }
        }
    }
}

// Longest path dfs (Hamiltonian path approximation)
int Bot::longestPathDFS(Location current, std::set<std::pair<int,int>> &visited) {
    visited.insert({current.w, current.h});
    
    int maxPath = 0;
    for (Location neighbor : getNeighbors(current)) {
        auto key = std::make_pair(neighbor.w, neighbor.h);
        if (isValidLocation(neighbor) && !game.getCrashed(neighbor) && 
            visited.find(key) == visited.end()) {
            int pathLength = longestPathDFS(neighbor, visited);
            maxPath = std::max(maxPath, pathLength);
        }
    }
    
    visited.erase({current.w, current.h});
    return maxPath + 1;
}