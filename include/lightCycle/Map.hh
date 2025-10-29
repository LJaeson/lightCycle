#pragma once
// #include "lightCycle.hh"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <deque>
#include <lightCycle/Map.hh>

#include <lightCycle/utility.hh>

// #include <lightCycle/Map.hh>
// #include <deque>
// #include <SFML/Graphics.hpp>
// #include <lightCycle/Game.hh>

// ---------- Tile ----------
struct Tile {
    Location location;
    TileColor tileColor;

    void changeTileColor(TileColor ac);
};

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

class Map {
private:
    MapTypes::Grid grid;
    renderQueue rq;

    int W;
    int H;

    sf::Color getTileColor(TileColor tileColor) const;


public:
    Map(int w, int h);

    void draw(sf::RenderTarget& window, int tileSize);
    void drawPart(sf::RenderTarget& window, int tileSize, Location loc);

    MapTypes::Grid getMap() const;
    Tile& getTile(int w, int h);
    Tile& getTile(Location l);

    void addLocation(Location l);
    bool haveLocationTask();
    Location getLocationQ();

    void createRandomWall();
    void changeTileColor(Location l, TileColor tileColor);
};
