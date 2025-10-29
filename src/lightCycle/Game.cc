#include <lightCycle/Game.hh>

// // ---------- Actor ----------
// actor::actor(Location l, TileColor ac) {
//     position.initPosition(l);
//     tileColor = ac;

//     if (tileColor == BLUE) actorColor = TileColor::BLUEACTOR;
//     else if (tileColor == GREEN) actorColor = TileColor::GREENACTOR;
// }

// bool actor::isDead(Map& map) {
//     return map.getTile(position.location).tileColor != NOPE;
// }

// void actor::changeTileBehind(Map& map) {
//     map.getTile(position.findPreLocation()).changeTileColor(tileColor);
//     map.addLocation(position.findPreLocation());
// }

// void actor::changeCurrentTile(Map& map) {
//     map.getTile(position.location).changeTileColor(actorColor);
//     map.addLocation(position.location);
// }

// void actor::doNextLocation() { position.doNextLocation(); }
// void actor::changeDirection(Direction d) { position.changeDirection(d); }

// // ---------- Player ----------
// player::player(Location l, TileColor ac) : actor(l, ac) {}

// ---------- Game ----------
Game::Game(int w, int h, Location p1Start, Location p2Start)
    : tickQueue{}, p1{p1Start, TileColor::BLUE}, p2{p2Start, TileColor::GREEN}, map{w,h} {}

void Game::tick() {
    tickQueue.addTask([this]{moveActor_();});
    tickQueue.addTask([this]{checkDeath_();});
    tickQueue.addTask([this]{modifyTile_();});
    tickQueue.executeTick();
}

void Game::modifyTile_() {
    p1.changeTileBehind(map);
    p2.changeTileBehind(map);
    p1.changeCurrentTile(map);
    p2.changeCurrentTile(map);
}

void Game::moveActor_() {
    p1.doNextLocation();
    p2.doNextLocation();
}

void Game::checkDeath_() {
    bool t1 = p1.isDead(map);
    bool t2 = p2.isDead(map);

    if (!t1 && !t2) return;

    if (t1 && t2) { std::cout << "Draw"; terminateCode = 1; }
    else if (t1)    { std::cout << "p2 win"; terminateCode = 2; }
    else if (t2)    { std::cout << "p1 win"; terminateCode = 3; }
}

void Game::draw(sf::RenderTarget& window, int tileSize) { map.draw(window, tileSize); }
void Game::drawPart(sf::RenderTarget& window, int tileSize, Location l) { map.drawPart(window, tileSize, l); }

Map& Game::getMap() { return map; }
Player& Game::getPlayer1() { return p1; }
Player& Game::getPlayer2() { return p2; }
int Game::getTerminateCode() { return terminateCode; }

bool Game::haveLocationTask() { return map.haveLocationTask(); }
Location Game::getLocationQ() { return map.getLocationQ(); }
