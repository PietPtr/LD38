#include <SFML/Graphics.hpp>
#include "Game.h"

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

    dt = clock.restart();
    totalTime += dt;

    if (frame % 100 == 0)
        sfx["test.wav"]->play();

    frame++;
}

void Game::draw()
{
    window->clear();

    Sprite test(textures["test.png"]);
    window->draw(test);

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
