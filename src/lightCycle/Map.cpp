#include <lightCycle/Map.hpp>

Map::Map(int w, int h) {
    grid.resize(w, std::vector<Tile>(h));

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            grid[x][y] = {{x,y}, TileColor::NOPE};
        }
    }

    for (int x = 0; x < w; x++) {
        grid[x][0].changeTileColor(TileColor::BOUNDARY);
        grid[x][h-1].changeTileColor(TileColor::BOUNDARY);
    }

    for (int y = 0; y < h; y++) {
        grid[0][y].changeTileColor(TileColor::BOUNDARY);
        grid[w-1][y].changeTileColor(TileColor::BOUNDARY);
    }
}

MapTypes::Grid Map::getMap() const {
    return grid;
}

Tile& Map::getTile(int w, int h) {
    return grid[w][h];
}

Tile& Map::getTile(Location l) {
    return grid[l.w][l.h];
}