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

        for (int x = 0; x < w; x++) {
            grid[x][0].changeTileColor(TileColor::BOUNDARY);
            grid[x][h-1].changeTileColor(TileColor::BOUNDARY);
        }

        for (int y = 0; y < h; y++) {
            grid[0][y].changeTileColor(TileColor::BOUNDARY);
            grid[w-1][y].changeTileColor(TileColor::BOUNDARY);
        }
    }

    //function

    //draw render function
    void draw(sf::RenderWindow& window, int tileSize) {
        sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));

        for (int w = 0; w < grid.size(); w++) {
            for (int h = 0; h < grid[w].size(); h++) {
                Tile& tile = grid[w][h];
                sf::Color color;  

                switch (tile.tileColor) {
                    case TileColor::BLUE:        color = sf::Color(0,190,172);  break;
                    case TileColor::GREEN:       color = sf::Color(132,178,42); break;
                    case TileColor::BOUNDARY:    color = sf::Color(219,0,0);    break;
                    case TileColor::NOPE:        color = sf::Color(0,0,0);      break;
                    case TileColor::GREENACTOR:  color = sf::Color(65,114,0);   break;
                    case TileColor::BLUEACTOR:   color = sf::Color(97,138,180); break;
                    default:                     color = sf::Color(0,0,0);      break;
                }
                
                rect.setFillColor(color);
                rect.setPosition(sf::Vector2f(w * tileSize, h * tileSize));
                window.draw(rect);
            }
        }
    }

    //getter
    MapTypes::Grid getMap() const {
        return grid;
    }

    Tile& getTile(int w, int h) {
        return grid[w][h];
    }

    Tile& getTile(Location l) {
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
    bool isDead(Map& map) {
        if (map.getTile(position.location).tileColor != TileColor::NOPE) {
            return true;
        }

        return false;
    }

    void changeTileBehind(Map& map) {
        map.getTile(position.findPreLocation()).changeTileColor(actorColor);

    }



    //accessor
    void doNextLocation() {
        position.doNextLocation();
    }

    void changeDirection(Direction d) {
        position.changeDirection(d);
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

        tickQueue.executeTick();
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
                // std::cout<<"1";
            return;
        } else {
            bool t1 = p1.isDead(map);
            bool t2 = p2.isDead(map);

            if (t1 && t2) {
                //draw
                std::cout<<"1";
                exit(0);
            } else if (t1) {
                //p2 win
                // std::cout<<"1";
                exit(0);
            } else if (t2) {
                //p1 win
                // std::cout<<"1";
                exit(0);
            }
        }

    }

    //function
    void userInput() {

    }

    void draw(sf::RenderWindow& window, int tileSize) {
        map.draw(window, tileSize);
    }

    //accessor
    Map& getMap(){
        return map;
    }

    player& getPlayer1() {
        return p1;
    }

    player& getPlayer2() {
        return p2;
    }
    
};

// lightCycle::lightCycle(/* args */)
// {
// }

// lightCycle::~lightCycle()
// {
// }

int main() {
    // std::cout << "hello world";

    sf::RenderWindow window(sf::VideoMode({800, 800}), "Light Cycle");
    window.setFramerateLimit(60);

    Game game(100, 100, Location{2, 0}, Location{97,0});

    sf::Clock clock;
    double accumulator = 0.0;
    const double TICK_STEP = 1000.0;


    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // close the window
            if (event->is<sf::Event::Closed>()) {
                window.close();
                // std::cout << "hello world"; 
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                 switch (keyPressed->scancode) {
                // --- Player 1 (Arrow Keys)
                case sf::Keyboard::Scan::Up:
                    game.getPlayer1().changeDirection(Direction::UP);
                    break;
                case sf::Keyboard::Scan::Left:
                    game.getPlayer1().changeDirection(Direction::LEFT);
                    break;
                case sf::Keyboard::Scan::Down:
                    game.getPlayer1().changeDirection(Direction::DOWN);
                    break;
                case sf::Keyboard::Scan::Right:
                    game.getPlayer1().changeDirection(Direction::RIGHT);
                    break;

                // --- Player 2 (WASD)
                case sf::Keyboard::Scan::W:
                    game.getPlayer2().changeDirection(Direction::UP);
                    break;
                case sf::Keyboard::Scan::A:
                    game.getPlayer2().changeDirection(Direction::LEFT);
                    break;
                case sf::Keyboard::Scan::S:
                    game.getPlayer2().changeDirection(Direction::DOWN);
                    break;
                case sf::Keyboard::Scan::D:
                    game.getPlayer2().changeDirection(Direction::RIGHT);
                    break;

                default:
                    break;
                }
            }
        }

        float delta = clock.restart().asMilliseconds();
        accumulator += delta;

        while (accumulator >= TICK_STEP) {
            game.tick();
            accumulator -= TICK_STEP;
            
        }

        if (window.hasFocus()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) game.getPlayer1().changeDirection(Direction::UP);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) game.getPlayer1().changeDirection(Direction::LEFT);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) game.getPlayer1().changeDirection(Direction::DOWN);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) game.getPlayer1().changeDirection(Direction::RIGHT);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) game.getPlayer2().changeDirection(Direction::UP);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) game.getPlayer2().changeDirection(Direction::LEFT);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) game.getPlayer2().changeDirection(Direction::DOWN);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) game.getPlayer2().changeDirection(Direction::RIGHT);
        }



        window.clear();
        //render
        game.draw(window, 4);
        window.display();



    }

    // Game game = Game(10, 10, Location{2, 0}, Location{7, 0});


    return 0;
}