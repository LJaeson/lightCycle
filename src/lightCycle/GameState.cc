#include <lightCycle/GameState.hh>

GameState::GameState(int w, int h) {
    this->w = w;
    this->h = h;
    grid.resize(w, std::vector<TileColor>(h));
}

GameState::GameState(Location l) {
    this->w = l.getW();
    this->h = l.getW();
    grid.resize(w, std::vector<TileColor>(h));
}

void GameState::copyGame(Game game, TileColor botColor, TileColor opColor) {
    MapTypes::Grid gameMap = game.getMapGrid();

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

std::vector<Direction> GameState::getPossibleMove(Position &pos) {
    std::vector<Direction> moves;
    // std::cout << pos.location.w << " " << pos.location.h << std::endl;
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