#include <movable/Bot.hh>
#include <lightCycle/GameState.hh>

#include <limits>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <queue>
#include <set>

#define MAX_DEPTH 10
#define USE_ALPHA_BETA_PRUNING true

Bot::Bot(Location l, Location mapSize, TileColor ac)
    : Actor(l, ac),
    tempGameState(std::make_unique<GameState>(mapSize))
{
    srand(time(0));
}


BotPlayingReturn Bot::getMove(const Game &gameState, TileColor color1, TileColor color2, double timeLimit) {
    sf::Clock clock;
    this->timeLimit = timeLimit;
    tempGameState->copyGame(gameState, color1, color2);
    // tempGameState->copyGame(gameState, color1, color2);
    
    int position = 0;
    Direction bestMove = Direction::DOWN;
    double maxEval = -std::numeric_limits<double>::infinity();
    double alpha = -std::numeric_limits<double>::infinity();
    double beta = std::numeric_limits<double>::infinity();
    
    // pre-filter deadly moves
    std::vector<Direction> possibleMoves = tempGameState->getPossibleMove(tempGameState->bot);
    std::vector<std::pair<Direction, double>> safeMoves;
    
    int currentSpace = floodFillCountFast(tempGameState->bot.location, 120);
    
    for (Direction dir : possibleMoves) {
        tempGameState->move(tempGameState->bot, dir);
        
        // check if this move leads to immediate death or trap
        int movesAfter = tempGameState->getPossibleMove(tempGameState->bot).size();
        int spaceAfter = floodFillCountFast(tempGameState->bot.location, 100);
        
        double safetyScore = 0.0;
        
        // 0 moves left 
        if (movesAfter == 0) {
            safetyScore = -100000.0;  
        }
        // 1 move left
        else if (movesAfter == 1) {
            safetyScore = -10000.0;  
        }
        // 2 moves left
        else if (movesAfter == 2) {
            safetyScore = -5000.0; 
        }
        // 3 moves left
        else if (movesAfter == 3) {
            safetyScore = -1000.0; 
        }
        // space reduction scenarios
        else if (spaceAfter < 10 && currentSpace > 30) {
            safetyScore = -8000.0;  
        }
        else if (spaceAfter < 15 && currentSpace > 40) {
            safetyScore = -5000.0;  
        }
        else if (spaceAfter < 20 && currentSpace > 50) {
            safetyScore = -3000.0;  
        }
        else if (spaceAfter < 30 && currentSpace > 60) {
            safetyScore = -1000.0;  
        }
        // reward keeping space
        else {
            if (spaceAfter > currentSpace * 0.8) {
                safetyScore = 1000.0;  
            } else if (spaceAfter > currentSpace * 0.6) {
                safetyScore = 500.0;  
            } else {
                safetyScore = 0.0; 
            }
        }
        
        safeMoves.push_back({dir, safetyScore});
        tempGameState->unmove();
    }
    
    // sort moves by safety score (best -> worst)
    std::sort(safeMoves.begin(), safeMoves.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // evaluate moves using minimax, prioritise safer moves
    for (const auto& [dir, safetyScore] : safeMoves) {
        if (exceedTimeLimit(clock)) {
            return { bestMove, position, clock.getElapsedTime().asMilliseconds()};
        }
        
        // skip deadly moves unless only option
        if (safetyScore <= -100000.0 && safeMoves.size() > 1) {
            continue; 
        }
        // skip almost-deadly moves if we have safer options
        if (safetyScore <= -5000.0 && safeMoves.size() > 2) {
            continue;  
        }
        // skip risky moves if we have 2+ safer options
        if (safetyScore <= -1000.0 && safeMoves.size() > 3) {
            continue; 
        }
        
        tempGameState->move(tempGameState->bot, dir);
        double eval = minimax(MAX_DEPTH, false, clock, position, alpha, beta);
        
        // safety score bonus
        eval += safetyScore;
        
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = dir;
        } else if (eval == maxEval && rand() % 2 == 1) {
            bestMove = dir;
        }
        alpha = std::max(alpha, eval);
        tempGameState->unmove();
    }
    std::cout << position << std::endl;
    return { bestMove, position, clock.getElapsedTime().asMilliseconds() };
}

bool Bot::botCanNotMove() {
    return tempGameState->getPossibleMove(tempGameState->bot).empty();

};

bool Bot::oponentCanNotMove() {
    return tempGameState->getPossibleMove(tempGameState->oponent).empty();
}

bool Bot::isTerminal() {
    return isDraw() || tempGameState->getCrashed(tempGameState->bot.location) || tempGameState->getCrashed(tempGameState->oponent.location);
};

bool Bot::exceedTimeLimit(sf::Clock &clock) {
    return clock.getElapsedTime().asMilliseconds() >= timeLimit;
}

double Bot::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position, double alpha, double beta) {
    if (isTerminal() || depth == 0 || exceedTimeLimit(clock)) {
        return evaluate();
    }
    ++position;
    if (maximizingPlayer) {
        double maxEval = -std::numeric_limits<double>::infinity();
        for (Direction dir: tempGameState->getPossibleMove(tempGameState->bot)) {
            tempGameState->move(tempGameState->bot, dir);
            maxEval = std::max(maxEval, minimax(depth - 1, false, clock, position, alpha, beta));
            tempGameState->unmove();
            alpha = std::max(alpha, maxEval);
            if (beta <= alpha) {
                break; // beta cutoff - opp. won't allow this path
            }
        }
        return maxEval;

    } else {
        double minEval = std::numeric_limits<double>::infinity();
        for (Direction dir: tempGameState->getPossibleMove(tempGameState->oponent)) {
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

bool Bot::isDraw() {
    return (tempGameState->getCrashed(tempGameState->bot.location) 
           && tempGameState->getCrashed(tempGameState->oponent.location)) 
           || tempGameState->bot.location.equal(tempGameState->oponent.location);
}

double Bot::evaluate() {
    if (isDraw()) {
        return -50; // draw
    } else if (tempGameState->getCrashed(tempGameState->bot.location)) {
        return -100; // oponent win
    } else if (tempGameState->getCrashed(tempGameState->oponent.location)) {
        return 100;
    }

    return evaluateSurvival();
}

// ==================== SURVIVAL ====================
// 
// used in evaluateSurvival:
// 1. flood fill (BFS) - space estimation with 100-120 iteration limits
// 2. stage detection - board fill ratio + component separation
// 3. early game : voronoi partitioning (multi-source BFS, 100 iterations)
// 4. mid game: articulation point detection (DFS chokepoint check + look-ahead)
// 5. end game: space filling maximises reachable territory
// 6. self-trap prevention: look-ahead checking space after each move
// 7. board coverage: encourages using entire board, no clustering
//
// ============================================================

// helper: calculate which quadrants the bot has covered, promotes coveriing more area
double Bot::evaluateBoardCoverage() {
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

// ==================== OPTIMISED FAST VERSIONS OF ALGORITHMS ====================

// voronoi: better territory assessment, more iterations
double Bot::evaluateVoronoiFast() {
    std::queue<std::pair<Location, int>> bfsQueue;
    std::vector<std::vector<int>> distances(tempGameState->w, std::vector<int>(tempGameState->h, -1));
    std::vector<std::vector<int>> owner(tempGameState->w, std::vector<int>(tempGameState->h, -1));
    
    // BFS from both players simultaneously
    bfsQueue.push({tempGameState->bot.location, 0});
    bfsQueue.push({tempGameState->oponent.location, 1});
    distances[tempGameState->bot.location.w][tempGameState->bot.location.h] = 0;
    distances[tempGameState->oponent.location.w][tempGameState->oponent.location.h] = 0;
    owner[tempGameState->bot.location.w][tempGameState->bot.location.h] = 0;
    owner[tempGameState->oponent.location.w][tempGameState->oponent.location.h] = 1;
    
    int botTerritory = 0;
    int opTerritory = 0;
    int maxIterations = 100; 
    int iterations = 0;
    
    while (!bfsQueue.empty() && iterations < maxIterations) {
        auto [loc, playerID] = bfsQueue.front();
        bfsQueue.pop();
        iterations++;
        
        for (Location neighbor : getNeighbors(loc)) {
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
                int newDist = distances[loc.w][loc.h] + 1;
                
                if (distances[neighbor.w][neighbor.h] == -1) {
                    distances[neighbor.w][neighbor.h] = newDist;
                    owner[neighbor.w][neighbor.h] = playerID;
                    bfsQueue.push({neighbor, playerID});
                    
                    if (playerID == 0) botTerritory++;
                    else opTerritory++;
                } else if (distances[neighbor.w][neighbor.h] == newDist && owner[neighbor.w][neighbor.h] != playerID) {
                    owner[neighbor.w][neighbor.h] = -1;  
                    if (playerID == 0 && botTerritory > 0) botTerritory--;
                    else if (playerID == 1 && opTerritory > 0) opTerritory--;
                }
            }
        }
    }
    
    return (double)(botTerritory - opTerritory);
}

// articulation points: check chokepoints with look-ahead
double Bot::evaluateArticulationPointsFast() {
    double score = 0.0;
    
    // check if current position is a chokepoint 
    if (isArticulationPoint(tempGameState->bot.location)) {
        score -= 30.0;  // penalise for being in dangerous chokepoint
    }
    
    // reward for multiple escape routes 
    int availableMoves = tempGameState->getPossibleMove(tempGameState->bot).size();
    score += availableMoves * 12.0;
    
    // look ahead at space after each move to avoid trapping
    int bestSpaceAfterMove = 0;
    int worstSpaceAfterMove = 999;
    for (Direction dir : tempGameState->getPossibleMove(tempGameState->bot)) {
        tempGameState->move(tempGameState->bot, dir);
        int spaceAfterMove = floodFillCountFast(tempGameState->bot.location, 80);
        bestSpaceAfterMove = std::max(bestSpaceAfterMove, spaceAfterMove);
        worstSpaceAfterMove = std::min(worstSpaceAfterMove, spaceAfterMove);
        tempGameState->unmove();
    }
    
    // reward moves that keep space available
    score += bestSpaceAfterMove * 0.8;
    
    // penalise if all moves lead to low space
    if (worstSpaceAfterMove < 10 && availableMoves > 1) {
        score -= 25.0;
    }
    
    // check if opponent is in a chokepoint
    if (isArticulationPoint(tempGameState->oponent.location)) {
        score += 20.0;
    }
    
    return score;
}

// longest path: fast flood fill instead of DFS
double Bot::evaluateLongestPathFast() {
    // approx longest path with reachable space count
    int botSpace = floodFillCountFast(tempGameState->bot.location, 100);  
    int opSpace = floodFillCountFast(tempGameState->oponent.location, 100);
    
    // In endgame, heavily prioritize having more space
    return (double)(botSpace - opSpace);
}

// ==================== SURVIVAL MODE IMPLEMENTATION ====================

// some current problems: bot can be little dumb, it traps itself, 
// and towards endgame the bot goes out of bounds 
// not used in 30x30 because it lags too much, but the results of this were better 
// and more 'optimal' (filled more spaces) than the optimised survival mode above

Bot::GameStage Bot::determineGameStage() {
    // count total occupied squares
    int totalOccupied = 0;
    int totalSquares = tempGameState->w * tempGameState->h;
    
    for (int i = 0; i < tempGameState->w; ++i) {
        for (int j = 0; j < tempGameState->h; ++j) {
            if (tempGameState->grid[i][j] == TileColor::BOUNDARY) {
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

// optimised versions are used here 
double Bot::evaluateSurvival() {
    GameStage stage = determineGameStage();
    double score = 0.0;
    
    // calcualte once, use multiple times for better performance 
    int botMoves = tempGameState->getPossibleMove(tempGameState->bot).size();
    int opMoves = tempGameState->getPossibleMove(tempGameState->oponent).size();
    double dist = distanceToOpponent();
    
    // aAlways evaluate reachable space to avoid self-trapping
    int botSpace = floodFillCountFast(tempGameState->bot.location, 120);
    int opSpace = floodFillCountFast(tempGameState->oponent.location, 120);
    
    // self trap detection with look ahead 
    // look at space, mobility avail after each possible move
    int maxSpaceAfterMove = 0;
    int maxMovesAfterMove = 0;
    int minSpaceAfterMove = 999;
    int minMovesAfterMove = 999;
    
    // count how many moves lead to danger
    int deadlyMoves = 0;
    int dangerousMoves = 0;
    int totalPossibleMoves = tempGameState->getPossibleMove(tempGameState->bot).size();
    
    for (Direction dir : tempGameState->getPossibleMove(tempGameState->bot)) {
        tempGameState->move(tempGameState->bot, dir);
        int spaceAfter = floodFillCountFast(tempGameState->bot.location, 120);
        int movesAfter = tempGameState->getPossibleMove(tempGameState->bot).size();
        
        maxSpaceAfterMove = std::max(maxSpaceAfterMove, spaceAfter);
        maxMovesAfterMove = std::max(maxMovesAfterMove, movesAfter);
        minSpaceAfterMove = std::min(minSpaceAfterMove, spaceAfter);
        minMovesAfterMove = std::min(minMovesAfterMove, movesAfter);
        
        // count risky moves
        if (movesAfter == 0) {
            deadlyMoves++;
        } else if (movesAfter <= 2) {
            dangerousMoves++;
        }
        
        tempGameState->unmove();
    }
    
    // penalise based on how many bad options we have 
    
    // move that leads to immediate death
    if (deadlyMoves > 0) {
        score -= deadlyMoves * 15000.0; 
    }
    
    // dangerous moves (1-2 mobility after)
    if (dangerousMoves > 0) {
        score -= dangerousMoves * 500.0;  
    }
    
    // if best option still has low mobility
    if (maxMovesAfterMove <= 2) {
        score -= 2000.0;  
    } else if (maxMovesAfterMove == 3) {
        score -= 800.0;   
    }
    
    // if all moves lead to low space, cornered
    if (minSpaceAfterMove < 20 && botSpace > 40) {
        score -= 3000.0;  
    } else if (minSpaceAfterMove < 30 && botSpace > 60) {
        score -= 1500.0;  
    }
    
    // if our best move still loses lots of space
    if (maxSpaceAfterMove < botSpace * 0.5 && botSpace > 30) {
        score -= 2000.0;  
    } else if (maxSpaceAfterMove < botSpace * 0.7 && botSpace > 50) {
        score -= 1000.0;  
    }
    
    // prio 1: claim space 
    score += (botSpace - opSpace) * 12.0;
    
    // reward the move that keeps maximum space open
    score += maxSpaceAfterMove * 3.0;  
    
    // prio 2: avoid dead ends
    score += (botMoves - opMoves) * 18.0;
    
    // reward future mobility 
    score += maxMovesAfterMove * 20.0;  
    
    // penalties for low mobility
    if (botMoves <= 1) {
        score -= 300.0;  
    } else if (botMoves == 2) {
        score -= 120.0; 
    } else if (botMoves == 3) {
        score -= 40.0;   
    }
    
    // prio 3: avoid collision
    // penalties for being close 
    if (dist < 2.0) {
        score -= 150.0;  
    } else if (dist < 3.0) {
        score -= 80.0;  
    } else if (dist < 5.0) {
        score -= 30.0;   
    } else if (dist < 8.0) {
        score -= 10.0;  
    }
    
    // reward  maintaining good distance
    if (dist > 8.0) {
        score += dist * 0.8;  
    } else if (dist > 5.0) {
        score += dist * 0.3;
    }
    
    // Penalty for very low space
    if (botSpace < 20) {
        score -= (20 - botSpace) * 15.0;
    }
    
    // encourage board coverage, prevent clustering
    score += evaluateBoardCoverage() * 3.5; 
    
    // === stage-specific eval ===
    switch (stage) {
        case EARLY_GAME:
            // voronoi: claim as much space as possible
            score += evaluateVoronoiFast() * 3.0;
            score += botSpace * 1.0;
            // encourage separation to different quadrants
            score += evaluateBoardCoverage() * 4.0;  
            // avoid opponent more aggressively
            if (dist < 10.0) {
                score -= (10.0 - dist) * 8.0;  
            }
            break;
            
        case MID_GAME:
            // articulation points: maintain escape routes
            score += evaluateArticulationPointsFast() * 3.0;
            score += botSpace * 1.0;
            break;
            
        case END_GAME:
            // longest path: maximise space filling in own territory
            score += botSpace * 2.5;
            // avoid the opponent 
            if (dist < 5.0) {
                score -= (5.0 - dist) * 25.0;
            }
            break;
    }
    
    return score;
}

// early game: voronoi-based territory evaluation using multi-source bfs
// simultaneous bfs from both players to partition board into territories
double Bot::evaluateVoronoi() {
    std::set<std::pair<int,int>> visited;
    std::vector<std::vector<int>> distances(tempGameState->w, std::vector<int>(tempGameState->h, -1));
    
    // bfs from both players to calculate voronoi regions
    std::queue<std::pair<Location, int>> bfsQueue;
    std::vector<std::vector<int>> owner(tempGameState->w, std::vector<int>(tempGameState->h, -1)); // 0=bot, 1=oponent, -1=unowned
    
    // tart bfs from both players simultaneously
    bfsQueue.push({tempGameState->bot.location, 0});
    bfsQueue.push({tempGameState->oponent.location, 1});
    distances[tempGameState->bot.location.w][tempGameState->bot.location.h] = 0;
    distances[tempGameState->oponent.location.w][tempGameState->oponent.location.h] = 0;
    owner[tempGameState->bot.location.w][tempGameState->bot.location.h] = 0;
    owner[tempGameState->oponent.location.w][tempGameState->oponent.location.h] = 1;
    
    int botTerritory = 0;
    int opTerritory = 0;
    int maxIterations = 150; // made it small to avoid lagging 
    int iterations = 0;
    
    while (!bfsQueue.empty() && iterations < maxIterations) {
        auto [loc, playerID] = bfsQueue.front();
        bfsQueue.pop();
        iterations++;
        
        for (Location neighbor : getNeighbors(loc)) {
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
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
    if (isArticulationPoint(tempGameState->bot.location)) {
        score -= 20.0; // avoid narrow spaces
    }
    
    // reward positions with high connectivity
    int reachableSquares = countReachableSquares(tempGameState->bot.location, std::set<std::pair<int,int>>());
    score += reachableSquares * 0.5;
    
    // check immediate neighbors 
    for (Location neighbor : getNeighbors(tempGameState->bot.location)) {
        if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
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
    int reachableSquares = floodFillCount(tempGameState->bot.location);
    
    // reward based on reachable space
    return reachableSquares * 2.0;
}

// check if players are in different connected components
// bfs from bot position to check if opponent is reachable
bool Bot::arePlayersInDifferentComponents() {
    std::set<std::pair<int,int>> visited;
    std::queue<Location> q;
    
    q.push(tempGameState->bot.location);
    visited.insert({tempGameState->bot.location.w, tempGameState->bot.location.h});
    
    while (!q.empty()) {
        Location current = q.front();
        q.pop();
        
        // if opponent reached, they're in the same component
        if (current.equal(tempGameState->oponent.location)) {
            return false;
        }
        
        for (Location neighbor : getNeighbors(current)) {
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
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
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
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

// fast flood fill with iteration limit
int Bot::floodFillCountFast(Location start, int maxIter) {
    std::set<std::pair<int,int>> visited;
    std::queue<Location> q;
    
    q.push(start);
    visited.insert({start.w, start.h});
    int count = 0;
    
    while (!q.empty() && count < maxIter) {
        Location current = q.front();
        q.pop();
        count++;
        
        for (Location neighbor : getNeighbors(current)) {
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor)) {
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
    int dx = abs(tempGameState->bot.location.w - tempGameState->oponent.location.w);
    int dy = abs(tempGameState->bot.location.h - tempGameState->oponent.location.h);
    return (double)(dx + dy);
}

// check if location is valid
bool Bot::isValidLocation(Location loc) {
    return loc.w >= 0 && loc.w < tempGameState->w && loc.h >= 0 && loc.h < tempGameState->h;
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
            if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor) && 
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
    if (tempGameState->getCrashed(loc)) return false;
    
    // temporarily mark location as blocked
    TileColor originalColor = tempGameState->grid[loc.w][loc.h];
    tempGameState->grid[loc.w][loc.h] = TileColor::BOUNDARY;
    
    // count components in neighbours, limit search depth for performance
    std::set<std::pair<int,int>> globalVisited;
    int componentCount = 0;
    int maxSearchSize = 50; 
    
    for (Location neighbor : getNeighbors(loc)) {
        auto key = std::make_pair(neighbor.w, neighbor.h);
        if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor) && 
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
                    if (isValidLocation(nextNeighbor) && !tempGameState->getCrashed(nextNeighbor) && 
                        globalVisited.find(nextKey) == globalVisited.end()) {
                        globalVisited.insert(nextKey);
                        q.push(nextNeighbor);
                    }
                }
            }
            
            componentCount++;
            if (componentCount > 1) {
                // early exit bcs we already know it's an articulation point
                tempGameState->grid[loc.w][loc.h] = originalColor;
                return true;
            }
        }
    }
    
    // restore original state
    tempGameState->grid[loc.w][loc.h] = originalColor;
    
    // if removing this point creates more than 1 component, it's an articulation point
    return componentCount > 1;
}

// find articulation points via DFS
void Bot::findArticulationPoints(Location start, std::set<Location> &articulationPoints) {
    int checkRadius = 5;
    
    for (int w = std::max(0, tempGameState->bot.location.w - checkRadius); 
         w < std::min(tempGameState->w, tempGameState->bot.location.w + checkRadius); ++w) {
        for (int h = std::max(0, tempGameState->bot.location.h - checkRadius); 
             h < std::min(tempGameState->h, tempGameState->bot.location.h + checkRadius); ++h) {
            Location loc = {w, h};
            if (!tempGameState->getCrashed(loc) && isArticulationPoint(loc)) {
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
        if (isValidLocation(neighbor) && !tempGameState->getCrashed(neighbor) && 
            visited.find(key) == visited.end()) {
            int pathLength = longestPathDFS(neighbor, visited);
            maxPath = std::max(maxPath, pathLength);
        }
    }
    
    visited.erase({current.w, current.h});
    return maxPath + 1;
}



//////////////////////////////archived v1//////////////////////////////

// Bot::Bot(int w, int h): tempGame(w, h) {
//     srand(time(0));
// // 

// Direction Bot::getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) {
    
//     sf::Clock clock;
//     this->timeLimit = timeLimit;
//     tempGameState->copyGame(game, color1, color2);
    
//     int position = 0;
//     Direction bestMove = Direction::DOWN;
//     double maxEval = -std::numeric_limits<double>::infinity();
//     // std::cout << "time: " << clock.getElapsedTime().asMilliseconds() << std::endl;
//     for (Direction dir: tempGameState->getPossibleMove(tempGameState->bot)) {
//         // std::cout << "get Move loop: " << dir << std::endl;
//         if (exceedTimeLimit(clock)) {
//             return bestMove;
//         }
//         tempGameState->move(tempGameState->bot, dir);
//         double eval = minimax(MAX_DEPTH, false, clock, position);
//         if (eval > maxEval) {
//             maxEval = eval;
//             bestMove = dir;
//         } else if (eval == maxEval && rand() % 2 == 1) {
//             bestMove = dir;
//         }
//         tempGameState->unmove();
//     }
//     std::cout << position << std::endl;
//     return bestMove;
// }

// bool Bot::botCanNotMove() {
//     return tempGameState->getPossibleMove(tempGameState->bot).empty();

// };

// bool Bot::oponentCanNotMove() {
//     return tempGameState->getPossibleMove(tempGameState->oponent).empty();
// }

// bool Bot::isTerminal() {
//     return isDraw() || tempGameState->getCrashed(tempGameState->bot.location) || tempGameState->getCrashed(tempGameState->oponent.location);
// };

// bool Bot::exceedTimeLimit(sf::Clock &clock) {
//     return clock.getElapsedTime().asMilliseconds() >= timeLimit;
// }

// double Bot::minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position) {
//     if (isTerminal() || depth == 0 || exceedTimeLimit(clock)) {
//         return evaluate();
//     }
//     ++position;
//     if (maximizingPlayer) {
//         double maxEval = -std::numeric_limits<double>::infinity();
//         for (Direction dir: tempGameState->getPossibleMove(tempGameState->bot)) {
//             tempGameState->move(tempGameState->bot, dir);
//             maxEval = std::max(maxEval, minimax(depth - 1, false, clock, position));
//             tempGameState->unmove();
//         }
//         return maxEval;

//     } else {
//         double minEval = std::numeric_limits<double>::infinity();
//         for (Direction dir: tempGameState->getPossibleMove(tempGameState->oponent)) {
//             tempGameState->move(tempGameState->oponent, dir);
//             minEval = std::min(minEval, minimax(depth - 1, true, clock, position));
//             tempGameState->unmove();
//         }
//         return minEval;
//     }
// }

// bool Bot::isDraw() {
//     return (tempGameState->getCrashed(tempGameState->bot.location) 
//            && tempGameState->getCrashed(tempGameState->oponent.location)) 
//            || tempGameState->bot.location.equal(tempGameState->oponent.location);
// }

// double Bot::evaluate() {
//     if (isDraw()) {
//         return -50; // draw
//     } else if (tempGameState->getCrashed(tempGameState->bot.location)) {
//         return -100; // oponent win
//     } else if (tempGameState->getCrashed(tempGameState->oponent.location)) {
//         return 100;
//     }
//     int freeSpaceCount = 0;
//     for (int i = 0; i < Direction::LEFT; i++) {
//         Location l = tempGameState->bot.findNextLocationFromDirection((Direction) i);
//         if (tempGameState->grid[l.w][l.h] == TileColor::NOPE) {
//             ++freeSpaceCount;
//         }
//     }
//     return 10 * (5 - freeSpaceCount);
//     // return 5 * tempGameState.getPossibleMove(tempGameState.bot).size();
// }