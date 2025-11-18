#include <movable/Actor.hh>

Actor::Actor(Location l, TileColor ac) {
    position.initPosition(l);
    tileColor = ac;

    if (tileColor == BLUE) actorColor = TileColor::BLUEACTOR;
    else if (tileColor == GREEN) actorColor = TileColor::GREENACTOR;
}

bool Actor::isDead(Map& map) {
    TileColor tileColor = map.getTile(position.location).tileColor;
    return tileColor != NOPE;
}

void Actor::changeTileBehind(Map& map) {
    map.getTile(position.findPreLocation()).changeTileColor(tileColor);
    map.addLocation(position.findPreLocation());
}

void Actor::changeCurrentTile(Map& map) {
    map.getTile(position.location).changeTileColor(actorColor);
    map.addLocation(position.location);
}

bool Actor::equalLocation(Actor &a) {
    return position.location.equal(a.position.location);
}

void Actor::doNextLocation() { position.doNextLocation(); }
void Actor::changeDirection(Direction d) { position.changeDirection(d); }
