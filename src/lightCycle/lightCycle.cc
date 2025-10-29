#include <lightCycle/lightCycle.hh>



class GameController {
private:
    // const unsigned int winW = 1000;
    // const unsigned int winH = 800;

    double accumulator = 0.0;   //dont change
    const double TICK_STEP = 800.0;
    const double BOT_LIMIT = 700.0;

    const int H = 15;
    const int W = 15;
    const unsigned int RenderBlockSize = 20;

    sf::RenderWindow window;
    sf::Font uiFont;
    Game game;

    //clock, how fast the game goes
    sf::Clock clock;

    //canva, to render graphic
    sf::RenderTexture canvas;


public:
    GameController(): 
    game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5,0}),
    window(sf::VideoMode({RenderBlockSize * W, RenderBlockSize * H}), "Light Cycle"),
    canvas({RenderBlockSize * W, RenderBlockSize * H})
    {};

    void playerInput(sf::Keyboard::Scancode key) {

        switch (key) {
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


    void run () {
        window.setFramerateLimit(60);

        canvas.clear();
        game.draw(canvas, RenderBlockSize);
        canvas.display();


        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                // close the window
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }


                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    playerInput(keyPressed->scancode);
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

    }


    void loadGameFont(std::string fontPath) {
        if (!uiFont.openFromFile(fontPath)) {
            std::cerr << "Failed to load font\n";
        }
    }

};


int main(int argc, char* argv[]) {

    GameController controller;

    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::string path = (exeDir / "resources" / "MinecraftRegular.otf").string();
    controller.loadGameFont(path);

    controller.run();

    return 0;
}