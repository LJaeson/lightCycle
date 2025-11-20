#include <lightCycle/GameState.hh>
#include <movable/Bot.hh>

GameState::GameState(int w, int h) {
    this->w = w;
    this->h = h;
    grid.resize(w, std::vector<TileColor>(h));
}

GameState::GameState(Location l) {
    this->w = l.getW();
    this->h = l.getH();
    grid.resize(w, std::vector<TileColor>(h));
}

void GameState::copyGame(const Game &game, TileColor botColor, TileColor opColor) {
    bot.initPosition(0, 0);
    oponent.initPosition(0, 0);
    MapTypes::Grid gameMap = game.getMapGrid();
    std::cout << w << std::endl;
    std::cout << h << std::endl;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            TileColor c = gameMap[i][j].tileColor;
            if (c == botColor) {
                bot.initPosition(i, j); 
            } else if (c == opColor) {
                oponent.initPosition(i, j);
            } else if (c != TileColor::NOPE) {
                grid[i][j] = TileColor::BOUNDARY;
            } else {
                grid[i][j] = TileColor::NOPE;
            }
        }
    }
}

bool GameState::getCrashed(Location l) {
    if (l.w < 0 || l.w >= w || l.h < 0 || l.h >= h) return true;
    
    return grid[l.w][l.h] == TileColor::BOUNDARY;
}

std::vector<Direction> GameState::getMove(Position &pos) {
    std::vector<Direction> moves;
    for (int i = 0; i <= Direction::LEFT; ++i) {
        moves.push_back((Direction) i);
    }
    return moves;
}

void GameState::move(Position &pos, Direction dir) {
    pos.direction = dir;

    grid[pos.location.w][pos.location.h] = TileColor::BOUNDARY;

    pos.doNextLocation();
    moveStack.push({&pos, dir});
}

void GameState::unmove() {
    Move move = moveStack.top();
    
    Position *pos = move.pos;
    Direction dir = move.dir;

    pos->direction = dir;

    pos->doPreLocation();
    
    Location l = pos->location;
    grid[l.w][l.h] = TileColor::NOPE;

    moveStack.pop();
}

std::vector<Direction> GameState::getPossibleMove(Position &pos) {
    std::vector<Direction> moves;
    for (int i = 0; i <= Direction::LEFT; ++i) {
        if (!getCrashed(pos.findNextLocationFromDirection((Direction) i))) {
            moves.push_back((Direction) i);
        }
    }
    return moves;
}