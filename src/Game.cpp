#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Collision.h"

using namespace sf;

Game* Game::instance;

Game::Game(RenderWindow* _window)
{
    Game::instance = this;
    window = _window;
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);

    view = View(Vector2f(0, 0), Vector2f(1280, 720));
    window->setView(view);

    player = new Player();

    textures["maze.png"].setRepeated(true);

    for (int i = 20; i < 360; i += DEGREES_PER_OBSTACLE)
    {
        restartClock();
        int deviation = randint(-OBSTACLE_POS_DEVIATION, OBSTACLE_POS_DEVIATION);
        obstacles.push_back(i + deviation);
    }
    std::cout << obstacles.size() << "\n";
}

void Game::resetGame()
{
    player = nullptr;
    obstacles.clear();

    initialize();
}

void Game::update()
{
    Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
            {
                window->close();
            }
            if (event.key.code == Keyboard::F1)
            {
                takeScreenshot();
            }
        }
        if (event.type == Event::LostFocus) {
            focus = false;
        }
        if (event.type == Event::GainedFocus) {
            focus = true;
        }

        if (event.type == Event::Resized) {
            windowWidth = event.size.width;
            windowHeight = event.size.height;
        }
    }

    restartClock();

    player->update(dt.asSeconds());

    view.setCenter(polarToVector(player->getMinDistance(), player->getAngle() + VIEW_ANGLE_OFFSET));
    view.setSize(16*5, 9*5);
    view.setRotation(player->getAngle() + 90 + VIEW_ANGLE_OFFSET);

    if (Keyboard::isKeyPressed(Keyboard::V)) {
        view.setCenter(0, 0);
        view.setSize(1280, 720);
        view.setRotation(0);
    }
    if (Keyboard::isKeyPressed(Keyboard::R)) {
        view.setRotation(0);
        view.setCenter(player->getPos());
    }

    int closestObstacle = 0;
    for (int i = obstacles.size() - 1; i >= 0; i--)
    {
        if ((int)fabs(obstacles[i].getAngle() - player->getAngle()) % 360 <
            (int)fabs(obstacles[closestObstacle].getAngle() - player->getAngle()) % 360)
        {
            closestObstacle = i;
        }

        obstacles[i].update(dt.asSeconds());
    }

    if (Collision::BoundingBoxTest(player->getSprite(), obstacles[closestObstacle].getSprite())) {
        player->kill(totalTime);
    }

    if (player->getDistance() < WORLDSIZE - 30)
    {
        resetGame();
    }

    frame++;
}

void Game::draw()
{
    window->clear(Color::White);

    window->setView(view);

    for (int i = 0; i < obstacles.size(); i++)
    {
        obstacles[i].draw(window);
    }

    CircleShape world = CircleShape(WORLDSIZE, 1024);
    world.setOrigin(Vector2f(WORLDSIZE, WORLDSIZE));
    world.setTexture(&textures["maze.png"]);
    world.setTextureRect({ 0, 0, 200, 200 });
    world.setOutlineColor(Color(0,0,0));
    world.setOutlineThickness(-4);
    window->draw(world);

    player->draw(window);

    window->display();
}

bool Game::isWindowOpen()
{
    return window->isOpen();
}

void Game::loadAudio(std::vector<std::string> audioFileNames)
{
    std::cout << "Loading " << audioFileNames.size() << " audio files..." << "\n";
    for (int i = 0; i < audioFileNames.size(); i++)
    {
        sfx.insert(std::pair<std::string, Audio*>(audioFileNames[i], new Audio()));
        sfx[audioFileNames[i]]->init("audio/" + audioFileNames[i]);
        std::cout << "Loaded audio " << "audio/" + audioFileNames[i] << "\n";
    }
}

void Game::loadTextures(std::vector<std::string> textureFileNames)
{
    std::cout << "Loading " << textureFileNames.size() << " textures...\n";
    for (int i = 0; i < textureFileNames.size(); i++)
    {
        Texture texture;
        std::string path = "textures/" + textureFileNames.at(i);
        if (!texture.loadFromFile(path))
            window->close();
        textures.insert(std::pair<std::string, Texture>(textureFileNames[i], texture));
        std::cout << "Loaded " << path << "\n";
    }
}

void Game::takeScreenshot()
{
    Image screenshot = window->capture();
    screenshot.saveToFile("./screenshot" + std::to_string(randint(0, 65546)) + ".png");
}

int Game::randint(int low, int high)
{
    int value = rand() % (high + 1 - low) + low;
    srand(totalTime.asMicroseconds() * value * rand());

    return value;
}

sf::Vector2f Game::polarToVector(float distance, float angle)
{
    sf::Vector2f result;
    result.x = cos(angle * 0.0174532925) * distance;
    result.y = sin(angle * 0.0174532925) * distance;

    return result;
}

void Game::restartClock()
{
    dt = clock.restart();
    totalTime += dt;
}
