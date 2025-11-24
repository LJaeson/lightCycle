#pragma once
// #include "lightCycle.hh"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <deque>
// #include <lightCycle/Map.hh>
#include <vector>
#include <lightCycle/utility.hh>

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
    int size() { return location_task_.size(); }
};

class Map {
private:
    MapTypes::Grid grid;
    renderQueue rq;

    int W;
    int H;

    


public:
    Map(int w, int h);

    void draw(sf::RenderTarget& window, int tileSizeW, int tileSizeH);
    void drawPart(sf::RenderTarget& window, int tileSizeW, int tileSizeH, Location loc);

    MapTypes::Grid getMap() const;
    Tile& getTile(int w, int h);
    Tile& getTile(Location l);

    sf::Color getTileColor(TileColor tileColor) const;
    void addLocation(Location l);
    bool haveLocationTask();
    Location getLocationQ();

    void createRandomWall();
    void changeTileColor(Location l, TileColor tileColor);
};
