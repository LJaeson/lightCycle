#pragma once
#include <lightCycle/lightCycle.hh>
// #include <lightCycle/Map.hh>
// #include <deque>
// #include <SFML/Graphics.hpp>

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
};
