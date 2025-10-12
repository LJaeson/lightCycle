#include <lightCycle/lightCycle.hh>



//taskQueue
class renderQueue {
protected:

    std::deque<Location> location_task_;
public:
    void addLocation(Location l) {
        location_task_.push_front(l);
    }

    bool haveLocationTask() {
        if(location_task_.empty()) {
            return false;
        }
        return true;
    }

    Location getLocationQ() {
        Location l = location_task_.back();
        location_task_.pop_back();
        return l;
    }
};

//Map class
class Map {
private:
    MapTypes::Grid grid;

    //for rending
    renderQueue rq;
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
    void draw(sf::RenderTarget& window, int tileSize) {
        sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));

        for (int w = 0; w < grid.size(); w++) {
            for (int h = 0; h < grid[w].size(); h++) {
                Tile& tile = grid[w][h];
                sf::Color color;  

                switch (tile.tileColor) {
                    case TileColor::NOPE:        color = sf::Color(0,0,0);      break;
                    case TileColor::BLUE:        color = sf::Color(0,190,172);  break;
                    case TileColor::GREEN:       color = sf::Color(132,178,42); break;
                    case TileColor::BOUNDARY:    color = sf::Color(219,0,0);    break;
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

    //only draw one tile
    void drawPart(sf::RenderTarget& window, int tileSize, Location location) {
        sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));

        int w = location.getW();
        int h = location.getH();

        Tile& tile = grid[w][h];
        sf::Color color;  

        switch (tile.tileColor) {
            case TileColor::NOPE:        color = sf::Color(0,0,0);      break;
            case TileColor::BLUE:        color = sf::Color(0,190,172);  break;
            case TileColor::GREEN:       color = sf::Color(132,178,42); break;
            case TileColor::BOUNDARY:    color = sf::Color(219,0,0);    break;
            case TileColor::GREENACTOR:  color = sf::Color(65,114,0);   break;
            case TileColor::BLUEACTOR:   color = sf::Color(97,138,180); break;
            default:                     color = sf::Color(0,0,0);      break;
        }
        
        rect.setFillColor(color);
        rect.setPosition(sf::Vector2f(w * tileSize, h * tileSize));
        window.draw(rect);

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

    void addLocation(Location l) {
        rq.addLocation(l);
    }

    bool haveLocationTask() {
        return rq.haveLocationTask();
    }

    Location getLocationQ() {
        return rq.getLocationQ();
    }

};


//actor class
class actor {
private:
    Position position;
    TileColor actorColor;
    TileColor tileColor;
public:
    //constructor

    actor(Location l, TileColor ac) {
        position.initPosition(l);
        tileColor = ac;

        if (tileColor == TileColor::BLUE) {
            actorColor = TileColor::BLUEACTOR;
        } else if (tileColor == TileColor::GREEN) {
            actorColor = TileColor::GREENACTOR;
        }
    }

    //function
    bool isDead(Map& map) {
        if (map.getTile(position.location).tileColor != TileColor::NOPE) {
            return true;
        }

        return false;
    }

    void changeTileBehind(Map& map) {
        map.getTile(position.findPreLocation()).changeTileColor(tileColor);

        map.addLocation(position.findPreLocation());
    }

    void changeCurrentTile(Map& map) {
        map.getTile(position.location).changeTileColor(actorColor);

        map.addLocation(position.location);
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

    player(Location l, TileColor ac) : actor(l, ac) {}

    //function

    // void 

};

//taskQueue
class taskQueue {
protected:
    using Task = std::function<void()>;

    std::deque<Task> task_;
    std::deque<Location> location_task_;
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

    void addLocation(Location l) {
        location_task_.push_front(l);
    }

    bool haveLocationTask() {
        if(location_task_.empty()) {
            return false;
        }
        return true;
    }

    Location getLocationQ() {
        Location l = location_task_.back();
        location_task_.pop_back();
        return l;
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

    int terminateCode = 0;

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

        tickQueue.executeTick();
    }

    //tickfunction
    void modifyTile_() {
        p1.changeTileBehind(map);
        p2.changeTileBehind(map);
        p1.changeCurrentTile(map);
        p2.changeCurrentTile(map);

    }

    void moveActor_() {
        p1.doNextLocation();
        p2.doNextLocation();
    }

    void checkDeath_() {
        if (!p1.isDead(map) && !p2.isDead(map)) {
            //keep game play
            return;
        } else {
            bool t1 = p1.isDead(map);
            bool t2 = p2.isDead(map);

            if (t1 && t2) {
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

    }

    //function
    void draw(sf::RenderTarget& window, int tileSize) {
        map.draw(window, tileSize);
    }

    void drawPart(sf::RenderTarget& window, int tileSize, Location l) {
        map.drawPart(window, tileSize, l);
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
    
    int getTerminateCode() {
        return terminateCode;
    }

    bool haveLocationTask() {
        return map.haveLocationTask();
    }

    Location getLocationQ() {
        return map.getLocationQ();
    }
};


int main(int argc, char* argv[]) {
    const int W = 450;
    const int H = 250;
    const int RenderBlockSize = 3;
    sf::RenderWindow window(sf::VideoMode({RenderBlockSize * W, RenderBlockSize * H}), "Light Cycle");
    window.setFramerateLimit(60);

    Game game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5,0});

    sf::RenderTexture canvas({RenderBlockSize * W, RenderBlockSize * H});

    canvas.clear();
    game.draw(canvas, RenderBlockSize);
    canvas.display();

    //load font
    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    sf::Font uiFont;
    if (!uiFont.openFromFile((exeDir / "resources" / "MinecraftRegular.otf").string())) {
        std::cerr << "Failed to load font\n";
    }

    //clock, how fast the game goes
    sf::Clock clock;
    double accumulator = 0.0;   //dont change
    const double TICK_STEP = 13.0; //this means 0.02s per tick

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // close the window
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }


            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                switch (keyPressed->scancode) {
                // --- Player 1 (WASD)
                case sf::Keyboard::Scan::W:
                    game.getPlayer1().changeDirection(Direction::UP);
                    break;
                case sf::Keyboard::Scan::A:
                    game.getPlayer1().changeDirection(Direction::LEFT);
                    break;
                case sf::Keyboard::Scan::S:
                    game.getPlayer1().changeDirection(Direction::DOWN);
                    break;
                case sf::Keyboard::Scan::D:
                    game.getPlayer1().changeDirection(Direction::RIGHT);
                    break;

                // --- Player 2 (Arrow Keys)
                case sf::Keyboard::Scan::Up:
                    game.getPlayer2().changeDirection(Direction::UP);
                    break;
                case sf::Keyboard::Scan::Left:
                    game.getPlayer2().changeDirection(Direction::LEFT);
                    break;
                case sf::Keyboard::Scan::Down:
                    game.getPlayer2().changeDirection(Direction::DOWN);
                    break;
                case sf::Keyboard::Scan::Right:
                    game.getPlayer2().changeDirection(Direction::RIGHT);
                    break;

                //restart the game
                case sf::Keyboard::Scan::Space:
                    if (game.getTerminateCode() != 0) {
                        game = Game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5,0}); 

                        clock.restart();    // reset game timer
                        accumulator = 0.0;  // reset tick timing
                        std::cout << "Game restarted" << std::endl;

                        canvas.clear();
                        game.draw(canvas, RenderBlockSize);
                        canvas.display();
                    }
                    break;

                default:
                    break;
                }
            }
        }

        //game tick and graphic handle
        float delta = clock.restart().asMilliseconds();
        accumulator += delta;

        if (game.getTerminateCode() == 0) {
            while (accumulator >= TICK_STEP) {
                game.tick();
                accumulator -= TICK_STEP;

                while (game.haveLocationTask()) {
                    game.drawPart(canvas, RenderBlockSize, game.getLocationQ());
                }
                canvas.display();
            }

        }

        window.clear();
        sf::Sprite frame(canvas.getTexture());
        window.draw(frame);


        //game over
        if (game.getTerminateCode() != 0) {

            sf::Text text(uiFont);
            text.setCharacterSize(50);
            text.setFillColor(sf::Color::White);

            switch (game.getTerminateCode()) {
                case 1: text.setString("Draw! Press SPACE to restart"); break;
                case 2: text.setString("Player 2 Wins! Press SPACE to restart"); break;
                case 3: text.setString("Player 1 Wins! Press SPACE to restart"); break;
            }

            //central the text
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(
                {bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f}
            );
            auto size = window.getSize();
            text.setPosition(
                {static_cast<float>(size.x) / 2.f,
                static_cast<float>(size.y) / 2.f}
            );

            window.draw(text);
        }

        window.display();
    }


    return 0;
}