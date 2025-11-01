#include <lightCycle/Game.hh>
#include <future>

// ---------- Game ----------
Game::Game(int w, int h, Location p1Start, Location p2Start)
    : tickQueue{}, 

    // p1(new Player(p1Start, TileColor::BLUE)),
    p1(new Bot(p1Start, TileColor::BLUE)),
    p2(new Bot(p2Start, TileColor::GREEN)),
    map{w,h} 
    {
        map.createRandomWall();
    }

void Game::tick() {
    tickQueue.addTask([this]{moveActor_();});
    tickQueue.addTask([this]{checkDeath_();});
    tickQueue.addTask([this]{modifyTile_();});
    tickQueue.addTask([this]{botPlaying_();});
    tickQueue.executeTick();
}

void Game::modifyTile_() {
    p1->changeTileBehind(map);
    p2->changeTileBehind(map);
    p1->changeCurrentTile(map);
    p2->changeCurrentTile(map);
}

void Game::moveActor_() {
    p1->doNextLocation();
    p2->doNextLocation();
}


void Game::botPlaying_() {
    if (p1->isPlayer() && p2->isPlayer()) return;
    sf::Clock clock;
    std::future<Direction> fut1;
    std::future<Direction> fut2;

    if (p1->isBot()) {
        fut1 = std::async(std::launch::async, [this]() {
            return p1->getMove(*this, getPlayer1().getColor(), getPlayer2().getColor(), BOT_LIMIT);
        });
    }

    if (p2->isBot()) {
        fut2 = std::async(std::launch::async, [this]() {
            return p2->getMove(*this, getPlayer2().getColor(), getPlayer1().getColor(), BOT_LIMIT);
        });
    }

    if (p1->isBot()) {
        getPlayer1().changeDirection(fut1.get());
        std::cout << "Bot 1 time: " << clock.getElapsedTime().asMilliseconds() << std::endl;
    }
    if (p2->isBot()) {
        getPlayer2().changeDirection(fut2.get());
        std::cout << "Bot 2 time: " << clock.getElapsedTime().asMilliseconds() << std::endl;
    }

    std::cout << "Total time: " << clock.getElapsedTime().asMilliseconds() << std::endl;
}

void Game::checkDeath_() {
    bool t1 = p1->isDead(map);
    bool t2 = p2->isDead(map);

    if (!t1 && !t2) return;

    if (t1 && t2) { std::cout << "Draw"; terminateCode = 1; }
    else if (t1)    { std::cout << "p2 win"; terminateCode = 2; }
    else if (t2)    { std::cout << "p1 win"; terminateCode = 3; }
}

void Game::draw(sf::RenderTarget& window, int tileSize) { map.draw(window, tileSize); }
void Game::drawPart(sf::RenderTarget& window, int tileSize, Location l) { map.drawPart(window, tileSize, l); }

Map& Game::getMap() { return map; }
Actor& Game::getPlayer1() { return *p1; }
Actor& Game::getPlayer2() { return *p2; }
int Game::getTerminateCode() { return terminateCode; }

bool Game::haveLocationTask() { return map.haveLocationTask(); }
Location Game::getLocationQ() { return map.getLocationQ(); }
