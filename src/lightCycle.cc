#include <lightCycle/lightCycle.hh>



//Map class
class Map {
private:
    MapTypes::Grid grid;
public:
    //constructor
    Map(int w, int h) {
        grid.resize(w, std::vector<Tile>(h));

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                grid[x][y] = {{x,y}, TileColor::NOPE};

            }
        }
    }

    //function

    //getter
    MapTypes::Grid getMap() const {
        return grid;
    }

    Tile getTile(int w, int h) {
        return grid[w][h];
    }

    Tile getTile(Location l) {
        return grid[l.w][l.h];
    }



};


//actor class
class actor {
private:
    Position position;
    TileColor actorColor;
public:
    //constructor

    actor(int w, int h, TileColor ac) {
        position.initPosition(w, h);
        actorColor = ac;
    }

    actor(Location l, TileColor ac) {
        position.initPosition(l);
        actorColor = ac;
    }

    //function
    bool isDead(Map map) {
        if (map.getTile(position.location).tileColor != TileColor::NOPE) {
            return true;
        }

        return false;
    }

    void changeTileBehind(Map map) {
        map.getTile(position.findPreLocation()).changeTileColor(actorColor);

    }

    //accessor
    void doNextLocation() {
        position.doNextLocation();
    }


};

class player : public actor{
private:
public:
    //constructor
    player(int w, int h, TileColor ac) : actor(w, h, ac) {}
    player(Location l, TileColor ac) : actor(l, ac) {}

    //function
    // void 

};

//taskQueue
class taskQueue {
protected:
    using Task = std::function<void()>;

    std::deque<Task> task_;
public:
    void addTask(Task fn) {
        task_.push_front(fn);
    }

    void executeTick() {
        while (!task_.empty()) {
            Task fn = task_.back();
            task_.pop_back();
            fn();
        }
    }
};
//game class, the big class that contain everything about game

class Game {
protected:
    taskQueue tickQueue;
    //for now, there is only human player, no npc, so we make two player
    player  p1;
    player  p2;

    Map map;

    bool runkillSwitch = true;


public:
    //constructor to init the game
    Game(int w, int h, Location p1Start, Location p2Start)
        : tickQueue{}        
        , p1{p1Start, TileColor::BLUE}   
        , p2{p2Start, TileColor::GREEN}   
        , map{w, h}     
    {}

    //tick
    void tick() {
        tickQueue.addTask([this]{moveActor_();});
        tickQueue.addTask([this]{checkDeath_();});
        tickQueue.addTask([this]{modifyTile_();});


        //change direction must after those
    }

    //tickfunction

    void modifyTile_() {
        p1.changeTileBehind(map);
        p2.changeTileBehind(map);

    }

    void moveActor_() {
        p1.doNextLocation();
        p2.doNextLocation();
    }

//TODO
    void checkDeath_() {
        if (!p1.isDead(map) && !p2.isDead(map)) {
            //keep game play
            return;
        } else {
            bool t1 = p1.isDead(map);
            bool t2 = p2.isDead(map);

            if (t1 && t2) {
                //draw
            } else if (t1) {
                //p2 win
            } else if (t2) {
                //p1 win
            }
        }

    }

    //function
    void userInput() {

    }
    
};

// lightCycle::lightCycle(/* args */)
// {
// }

// lightCycle::~lightCycle()
// {
// }

int main() {
    std::cout << "hello world";

    Game game = Game(10, 10, Location{2, 0}, Location{7, 0});


    return 0;
}