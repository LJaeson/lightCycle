#include <lightCycle/actor.hpp>

actor::actor(Location l, TileColor ac) {
    position.initPosition(l);
    tileColor = ac;

    if (tileColor == TileColor::BLUE) {
        actorColor = TileColor::BLUEACTOR;
    } else if (tileColor == TileColor::GREEN) {
        actorColor = TileColor::GREENACTOR;
    }
}

bool actor::isDead(Map& map) {
    return map.getTile(position.location).tileColor != TileColor::NOPE;
}

void actor::changeTileBehind(Map& map) {
    map.getTile(position.findPreLocation()).changeTileColor(tileColor);
}

void actor::changeCurrentTile(Map& map) {
    map.getTile(position.location).changeTileColor(actorColor);
}

//accessor
void actor::doNextLocation() {
    position.doNextLocation();
}

void actor::changeDirection(Direction d) {
    position.changeDirection(d);
}

const Position &actor::getPosition() const {
    return position;
}

TileColor actor::getColor() const { 
    return actorColor; 
}
