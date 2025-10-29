#include <SFML/Graphics/RectangleShape.hpp>
#include <lightCycle/Map.hh>

// ---------- Tile ----------
void Tile::changeTileColor(TileColor ac) { tileColor = ac; }

// ---------- RenderQueue ----------
void renderQueue::addLocation(Location l) { location_task_.push_front(l); }
bool renderQueue::haveLocationTask() const { return !location_task_.empty(); }
Location renderQueue::getLocationQ() {
    Location l = location_task_.back();
    location_task_.pop_back();
    return l;
}

// ---------- Map ----------
Map::Map(int w, int h) {
    grid.resize(w, std::vector<Tile>(h));

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++)
            grid[x][y] = {{x,y}, TileColor::NOPE};

        grid[x][0].changeTileColor(TileColor::BOUNDARY);
        grid[x][h-1].changeTileColor(TileColor::BOUNDARY);
    }

    for (int y = 0; y < h; y++) {
        grid[0][y].changeTileColor(TileColor::BOUNDARY);
        grid[w-1][y].changeTileColor(TileColor::BOUNDARY);
    }
}

sf::Color Map::getTileColor(TileColor tileColor) const {
    switch(tileColor) {
        case NOPE:       return sf::Color(0,0,0);
        case BLUE:       return sf::Color(0,190,172);
        case GREEN:      return sf::Color(132,178,42);
        case BOUNDARY:   return sf::Color(219,0,0);
        case GREENACTOR: return sf::Color(65,114,0);
        case BLUEACTOR:  return sf::Color(97,138,180);
        default:         return sf::Color(0,0,0);
    }
}

void Map::draw(sf::RenderTarget& window, int tileSize) {
    sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));
    for (int w = 0; w < grid.size(); w++) {
        for (int h = 0; h < grid[w].size(); h++) {
            rect.setFillColor(getTileColor(grid[w][h].tileColor));
            rect.setPosition(sf::Vector2f(w*tileSize, h*tileSize));
            window.draw(rect);
        }
    }
}

void Map::drawPart(sf::RenderTarget& window, int tileSize, Location loc) {
    Tile& tile = grid[loc.w][loc.h];
    sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));
    rect.setFillColor(getTileColor(tile.tileColor));
    rect.setPosition(sf::Vector2f(loc.w*tileSize, loc.h*tileSize));
    window.draw(rect);
}

MapTypes::Grid Map::getMap() const { return grid; }
Tile& Map::getTile(int w, int h) { return grid[w][h]; }
Tile& Map::getTile(Location l) { return grid[l.w][l.h]; }

void Map::addLocation(Location l) { rq.addLocation(l); }
bool Map::haveLocationTask() { return rq.haveLocationTask(); }
Location Map::getLocationQ() { return rq.getLocationQ(); }
