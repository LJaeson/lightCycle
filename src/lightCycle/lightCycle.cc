#include <lightCycle/lightCycle.hh>
#include <Player/PlayerFactory.hpp>
#include <Player/Player.hpp>
#include <lightCycle/actor.hpp>
#include <lightCycle/Map.hpp>
#include <lightCycle/Game.hpp>
#include <functional>

class GameController {
    private:
        const unsigned int winW = 1000;
        const unsigned int winH = 800;

        const int H = 15;
        const int W = 15;

        sf::RenderWindow window;
        sf::Font uiFont;

        std::unique_ptr<Player> player1;
        std::unique_ptr<Player> player2;
        Game game;

    public:
        GameController(std::string player1, std::string player2): 
            game(H, W, Location{1, 1}, Location{W - 2, 1}),
            window(sf::VideoMode({winW, winH}), "Light Cycle")
        {
            window.setFramerateLimit(60);
            this->player1 = PlayerFactory(player1);
            this->player2 = PlayerFactory(player2);
        }

        void drawGame() {
            const float renderBlockSizeW = (double) winW / W;
            const float renderBlockSizeH = (double) winH / H;

            sf::RectangleShape rect({renderBlockSizeW, renderBlockSizeH});
            MapTypes::Grid grid = game.getMapGrid();

            for (int w = 0; w < grid.size(); w++) {
                for (int h = 0; h < grid[w].size(); h++) {
                    Tile tile = grid[w][h];
                    sf::Color color;  

                    switch (tile.tileColor) {
                        case TileColor::NOPE:        color = sf::Color(255, 255, 255);      break;
                        case TileColor::BLUE:        color = sf::Color(0,190,172);  break;
                        case TileColor::GREEN:       color = sf::Color(132,178,42); break;
                        case TileColor::BOUNDARY:    color = sf::Color(0,0,0);    break;
                        case TileColor::GREENACTOR:  color = sf::Color(65,114,0);   break;
                        case TileColor::BLUEACTOR:   color = sf::Color(97,138,180); break;
                        default:                     color = sf::Color(0,0,0);      break;
                    }
                    
                    rect.setFillColor(color);
                    rect.setOutlineThickness(1.f);
                    rect.setOutlineColor(sf::Color(0,0,0));
                    rect.setPosition(sf::Vector2f(w * renderBlockSizeW, h * renderBlockSizeH));
                    window.draw(rect);
                }
            }
        }

        void loadGameFont(std::string fontPath) {
            if (!uiFont.openFromFile(fontPath)) {
                std::cerr << "Failed to load font\n";
            }
        }

        void clientMovementControl(sf::Keyboard::Scancode key, sf::Clock &clock, double &accumulator) {
            int player1 = -1;
            int player2 = -1;
            switch (key) {
            // --- Player 1 (WASD)
            case sf::Keyboard::Scan::W:
                player1 = Direction::UP;
                break;
            case sf::Keyboard::Scan::A:
                player1 = Direction::LEFT;
                break;
            case sf::Keyboard::Scan::S:
                player1 = Direction::DOWN;
                break;
            case sf::Keyboard::Scan::D:
                player1 = Direction::RIGHT;
                break;

            // --- Player 2 (Arrow Keys)
            case sf::Keyboard::Scan::Up:
                player2 = Direction::UP;
                break;
            case sf::Keyboard::Scan::Left:
                player2 = Direction::LEFT;
                break;
            case sf::Keyboard::Scan::Down:
                player2 = Direction::DOWN;
                break;
            case sf::Keyboard::Scan::Right:
                player2 = Direction::RIGHT;
                break;

            //restart the game
            case sf::Keyboard::Scan::Space:
                if (game.getTerminateCode() != 0) {
                    game = Game(W, H, Location{1, 1}, Location{W - 2, 1}); 

                    clock.restart();    // reset game timer
                    accumulator = 0.0;  // reset tick timing
                    std::cout << "Game restarted" << std::endl;

                    window.clear();
                    drawGame();
                }
                break;

            default:
                break;
            }

            if (player1 != -1 && this->player1->clientControlled()) {
                game.getPlayer1().changeDirection((Direction) player1);
            } else if (player2 != -1 && this->player2->clientControlled()) {
                game.getPlayer2().changeDirection((Direction) player2);
            }
        }

        void gameLoop() {
            //clock, how fast the game goes
            sf::Clock clock;
            double accumulator = 0.0;   //dont change
            const double TICK_STEP = 1000.0; //this means 0.02s per tick

            while (window.isOpen()) {
                while (const std::optional event = window.pollEvent()) {
                    // close the window
                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                    }

                    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                        clientMovementControl(keyPressed->scancode, clock, accumulator);
                    }
                }
                // Clear the window
                window.clear();

                //game tick and graphic handle
                float delta = clock.restart().asMilliseconds();
                accumulator += delta;

                if (game.getTerminateCode() == 0) {
                    while (accumulator >= TICK_STEP) {
                        game.tick();
                        accumulator -= TICK_STEP;
                    }

                }

                drawGame();
                
                //game over
                if (game.getTerminateCode() != 0) {

                    sf::Text text(uiFont);
                    text.setCharacterSize(50);
                    text.setFillColor(sf::Color::Red);

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
        }
};

int main(int argc, char* argv[]) {

    GameController controller("Client", "Client");

    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::string path = (exeDir / "resources" / "MinecraftRegular.otf").string();
    controller.loadGameFont(path);

    controller.gameLoop();
    // const int W = 450;
    // const int H = 250;
    // const int RenderBlockSize = 50;
    // sf::RenderWindow window(sf::VideoMode({RenderBlockSize * W, RenderBlockSize * H}), "Light Cycle");
    // window.setFramerateLimit(60);

    // Game game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5, 0});

    // sf::RenderTexture canvas({RenderBlockSize * W, RenderBlockSize * H});

    // canvas.clear();
    // game.draw(canvas, RenderBlockSize);
    // canvas.display();

    // //load font
    // std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    // sf::Font uiFont;
    // if (!uiFont.openFromFile((exeDir / "resources" / "MinecraftRegular.otf").string())) {
    //     std::cerr << "Failed to load font\n";
    // }

    // //clock, how fast the game goes
    // sf::Clock clock;
    // double accumulator = 0.0;   //dont change
    // const double TICK_STEP = 100.0; //this means 0.02s per tick

    // while (window.isOpen()) {
    //     while (const std::optional event = window.pollEvent()) {
    //         // close the window
    //         if (event->is<sf::Event::Closed>()) {
    //             window.close();
    //         }


    //         if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

    //             switch (keyPressed->scancode) {
    //             // --- Player 1 (WASD)
    //             case sf::Keyboard::Scan::W:
    //                 game.getPlayer1().changeDirection(Direction::UP);
    //                 break;
    //             case sf::Keyboard::Scan::A:
    //                 game.getPlayer1().changeDirection(Direction::LEFT);
    //                 break;
    //             case sf::Keyboard::Scan::S:
    //                 game.getPlayer1().changeDirection(Direction::DOWN);
    //                 break;
    //             case sf::Keyboard::Scan::D:
    //                 game.getPlayer1().changeDirection(Direction::RIGHT);
    //                 break;

    //             // --- Player 2 (Arrow Keys)
    //             case sf::Keyboard::Scan::Up:
    //                 game.getPlayer2().changeDirection(Direction::UP);
    //                 break;
    //             case sf::Keyboard::Scan::Left:
    //                 game.getPlayer2().changeDirection(Direction::LEFT);
    //                 break;
    //             case sf::Keyboard::Scan::Down:
    //                 game.getPlayer2().changeDirection(Direction::DOWN);
    //                 break;
    //             case sf::Keyboard::Scan::Right:
    //                 game.getPlayer2().changeDirection(Direction::RIGHT);
    //                 break;

    //             //restart the game
    //             case sf::Keyboard::Scan::Space:
    //                 if (game.getTerminateCode() != 0) {
    //                     game = Game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5,0}); 

    //                     clock.restart();    // reset game timer
    //                     accumulator = 0.0;  // reset tick timing
    //                     std::cout << "Game restarted" << std::endl;

    //                     canvas.clear();
    //                     game.draw(canvas, RenderBlockSize);
    //                     canvas.display();
    //                 }
    //                 break;

    //             default:
    //                 break;
    //             }
    //         }
    //     }

    //     //game tick and graphic handle
    //     float delta = clock.restart().asMilliseconds();
    //     accumulator += delta;

    //     if (game.getTerminateCode() == 0) {
    //         while (accumulator >= TICK_STEP) {
    //             game.tick();
    //             accumulator -= TICK_STEP;

    //             while (game.haveLocationTask()) {
    //                 game.drawPart(canvas, RenderBlockSize, game.getLocationQ());
    //             }
    //             canvas.display();
    //         }

    //     }

    //     window.clear();
    //     sf::Sprite frame(canvas.getTexture());
    //     window.draw(frame);


    //     //game over
    //     if (game.getTerminateCode() != 0) {

    //         sf::Text text(uiFont);
    //         text.setCharacterSize(50);
    //         text.setFillColor(sf::Color::White);

    //         switch (game.getTerminateCode()) {
    //             case 1: text.setString("Draw! Press SPACE to restart"); break;
    //             case 2: text.setString("Player 2 Wins! Press SPACE to restart"); break;
    //             case 3: text.setString("Player 1 Wins! Press SPACE to restart"); break;
    //         }

    //         //central the text
    //         sf::FloatRect bounds = text.getLocalBounds();
    //         text.setOrigin(
    //             {bounds.position.x + bounds.size.x / 2.f,
    //             bounds.position.y + bounds.size.y / 2.f}
    //         );
    //         auto size = window.getSize();
    //         text.setPosition(
    //             {static_cast<float>(size.x) / 2.f,
    //             static_cast<float>(size.y) / 2.f}
    //         );

    //         window.draw(text);
    //     }

    //     window.display();
    // }


    return 0;
}
