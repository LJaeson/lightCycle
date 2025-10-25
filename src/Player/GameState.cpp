#include <Player/GameState.hpp>

GameState::GameState(int w, int h) {
    this->w = w;
    this->h = h;
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
            } 
            
            if (c != TileColor::NOPE) {
                grid[i][j] = TileColor::BOUNDARY;
            } else {
                grid[i][j] = TileColor::NOPE;
            }
        }
    }
}

bool GameState::getCrashed(Location l) {
    if (grid[l.w][l.h] == TileColor::BOUNDARY) return true;
    return false; 
}

std::vector<Direction> GameState::getPossibleMove(Position &pos) {
    std::vector<Direction> moves;
    for (int i = 0; i <= Direction::LEFT; ++i) {
        if (pos.changeDirection((Direction) i) && !getCrashed(pos.findNextLocation())) {
            moves.push_back((Direction) i);
        }
    }
    return moves;
}

void GameState::move(Position &pos, Direction dir) {
    pos.changeDirection(dir);

    Location l = pos.findNextLocation();
    grid[l.w][l.h] = TileColor::BOUNDARY;

    pos.doNextLocation();

    moveStack.push({&pos, dir});
}

void GameState::unmove() {
    Move move = moveStack.top();
    
    Position *pos = move.pos;
    Direction dir = move.dir;

    pos->changeDirection(dir);

    Location l = pos->location;
    grid[l.w][l.h] = TileColor::NOPE;

    pos->doPreLocation();

    moveStack.pop();
}