#include <Player/Player.hpp>
#include <Player/GameState.hpp>
#include <set>
#include <queue>
#include <unordered_set>

class Bot: public Player {
    private:
        GameState game;
        double timeLimit;
        
        // added game stage tracking for survival
        enum GameStage {
            EARLY_GAME,   // < 30% board filled: voronoi territory control
            MID_GAME,     // 30-70% filled: articulation point avoidance
            END_GAME      // > 70% filled: space filling optimisation
        };
        
    public:
        Bot(int w, int h);

        bool clientControlled() override;

        Direction getMove(const Game &game, TileColor color1, TileColor color2, double timeLimit) override;

        bool botCanNotMove();
        bool oponentCanNotMove();

        bool isTerminal();

        double minimax(int depth, bool maximizingPlayer, sf::Clock &clock, int &position);

        // evaluation function to uses survival mode
        double evaluate();

        bool exceedTimeLimit(sf::Clock &clock);

        bool isDraw();
        
        // ====================  Survival mode functions ====================
        
        // determines current game stage based on board fill ratio
        GameStage determineGameStage();
        
        // early game: voronoi territory evaluation using multi-source bfs
        double evaluateVoronoi();
        int voronoiFloodFill(Location start, std::set<std::pair<int,int>> &visited, 
                             std::vector<std::vector<int>> &distances, int maxDist);
        
        // mid game: articulation point detection
        double evaluateArticulationPoints();
        int countReachableSquares(Location start, const std::set<std::pair<int,int>> &exclude);
        bool isArticulationPoint(Location loc);
        void findArticulationPoints(Location start, std::set<Location> &articulationPoints);
        
        // end game: space filling optimisation (simplified from longest path)
        double evaluateLongestPath();
        int longestPathDFS(Location current, std::set<std::pair<int,int>> &visited);
        
        // utility functions for survival mode
        bool arePlayersInDifferentComponents();  // bfs component detection
        int floodFillCount(Location start);      // count reachable squares
        double distanceToOpponent();              // manhattan distance calculation
        bool isValidLocation(Location loc);       // bounds checking
        std::vector<Location> getNeighbors(Location loc);  // Get 4-direction neighbours
        
        // main survival evaluation function (called by evaluate())
        double evaluateSurvival();
};