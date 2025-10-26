#include <lightCycle/Game.hpp>
#include <lightCycle/lightCycle.hh>

void taskQueue::addTask(Task fn) {
    task_.push_front(fn);
}

void taskQueue::executeTick() {
    while (!task_.empty()) {
        Task fn = task_.back();
        task_.pop_back();
        fn();
    }
}

void taskQueue::addLocation(Location l) {
    location_task_.push_front(l);
}

bool taskQueue::haveLocationTask() {
    if(location_task_.empty()) {
        return false;
    }
    return true;
}

Location taskQueue::getLocationQ() {
    Location l = location_task_.back();
    location_task_.pop_back();
    return l;
}

Game::Game(int w, int h, Location p1Start, Location p2Start)
    : tickQueue{}        
    , p1{p1Start, TileColor::BLUE}   
    , p2{p2Start, TileColor::GREEN}   
    , map{w, h}
{
    p1.changeCurrentTile(map);
    p2.changeCurrentTile(map);
}

    //tick
void Game::tick() {
    tickQueue.addTask([this]{moveActor_();});
    tickQueue.addTask([this]{checkDeath_();});
    tickQueue.addTask([this]{modifyTile_();});
    tickQueue.executeTick();
}

//tickfunction
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
    if (p1.equalLocation(p2) || (t1 && t2)) {
        //draw
        std::cout<<"Draw";
        terminateCode = 1;
        // exit(0);
    } else if (t1) {
        //p2 win
        std::cout<<"p2 win";
        terminateCode = 2;
        // exit(0);
    } else if (t2) {
        //p1 win
        std::cout<<"p1 win";
        terminateCode = 3;
        // exit(0);
    }
}

//accessor
Map& Game::getMap() {
    return map;
}

player& Game::getPlayer1() {
    return p1;
}

player& Game::getPlayer2() {
    return p2;
}

int Game::getTerminateCode() {
    return terminateCode;
}

// bool haveLocationTask() {
//     return map.haveLocationTask();
// }

// Location getLocationQ() {
//     return map.getLocationQ();
// }

MapTypes::Grid Game::getMapGrid() {
    return map.getMap();
}