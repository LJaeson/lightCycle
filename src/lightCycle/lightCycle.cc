#include <lightCycle/lightCycle.hh>
#include <thread>


const double BOT_LIMIT = 500.0;
bool windowOpen = true;
class GameController {
public:

private:

    double accumulator = 0.0;   //dont change
    const double TICK_STEP = 800.0;

    const int H = 30;
    const int W = 30;
    const unsigned int RenderBlockSize = 20;

    sf::RenderWindow window;
    sf::Font uiFont;
    Game game;

    //clock, how fast the game goes
    sf::Clock clock;

    //canva, to render graphic
    sf::RenderTexture canvas;


    static std::queue<sf::Keyboard::Scancode> inputQueue;
    static std::mutex queueMutex;

    static bool windowOpen;
public:
    GameController(): 
    game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5,0}),
    window(sf::VideoMode({RenderBlockSize * W, RenderBlockSize * H}), "Light Cycle"),
    canvas({RenderBlockSize * W, RenderBlockSize * H})
    {};
        

    

    void playerInput(sf::Keyboard::Scancode key) {  // or sf::Keyboard::Scancode if you prefer
        // --- Player 1 (WASD)
        if (game.getPlayer1().isPlayer()) {
            if (key == sf::Keyboard::Scancode::W) game.getPlayer1().changeDirection(Direction::UP);
            else if (key == sf::Keyboard::Scancode::A) game.getPlayer1().changeDirection(Direction::LEFT);
            else if (key == sf::Keyboard::Scancode::S) game.getPlayer1().changeDirection(Direction::DOWN);
            else if (key == sf::Keyboard::Scancode::D) game.getPlayer1().changeDirection(Direction::RIGHT);
        }

        // --- Player 2 (Arrow keys)
        if (game.getPlayer2().isPlayer()) {
            if (key == sf::Keyboard::Scancode::Up) game.getPlayer2().changeDirection(Direction::UP);
            else if (key == sf::Keyboard::Scancode::Left) game.getPlayer2().changeDirection(Direction::LEFT);
            else if (key == sf::Keyboard::Scancode::Down) game.getPlayer2().changeDirection(Direction::DOWN);
            else if (key == sf::Keyboard::Scancode::Right) game.getPlayer2().changeDirection(Direction::RIGHT);
        }

        // --- Restart game
        if (key == sf::Keyboard::Scancode::Space && game.getTerminateCode() != 0) {
            game = Game(W, H, Location{2 * W / 5, 0}, Location{3 * W / 5, 0}); 
            clock.restart();
            accumulator = 0.0;

            std::cout << "Game restarted" << std::endl;
            canvas.clear();
            game.draw(canvas, RenderBlockSize);
            canvas.display();
        }
    }


    void run () {
        window.setFramerateLimit(60);

        canvas.clear();
        game.draw(canvas, RenderBlockSize);
        canvas.display();

        while (window.isOpen()) {

            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    inputQueue.push(keyPressed->scancode);
                }
            }

            while (true) {

                sf::Keyboard::Scancode key;
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    if (inputQueue.empty()) break;
                    key = inputQueue.front();
                    inputQueue.pop();
                }
                playerInput(key); // your function
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

std::queue<sf::Keyboard::Scancode> GameController::inputQueue;
std::mutex GameController::queueMutex;
bool GameController::windowOpen = true;

int main(int argc, char* argv[]) {

    GameController controller;

    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::string path = (exeDir / "resources" / "MinecraftRegular.otf").string();
    controller.loadGameFont(path);

    controller.run();

    return 0;
}