#pragma once
// This file exists for compatibility with main branch's movable/ code
// Tile and other types are now defined in lightCycle.hh
#include <lightCycle/lightCycle.hh>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <deque>

// Tile is now defined in lightCycle.hh, skip redefinition

// ---------- Map ----------
namespace MapTypes {
    using Grid = std::vector<std::vector<Tile>>;
}

//taskQueue
class renderQueue {
protected:
    std::deque<Location> location_task_;
public:
    void addLocation(Location l);
    bool haveLocationTask() const;
    Location getLocationQ();
};

class Map : public renderQueue {
protected:
    MapTypes::Grid grid_;

    int W;
    int H;
public:
    Map(int w, int h);

    bool outOfBound(Location location);
    Tile& getTile(Location location);

    void changeTileColor(Location l, TileColor c);

    void draw(sf::RenderTarget& canvas, int renderBlockSize);

    void drawPart(sf::RenderTarget& canvas, int renderBlockSize, Location l);

    sf::Color getColor(TileColor color);

    int getWidth() {return W;};
    int getHeight() {return H;};
    MapTypes::Grid& getGrid() { return grid_; };
};
