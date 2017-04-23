#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>

using namespace sf;
int main()
{
    srand(time(NULL));

    RenderWindow window(VideoMode(1920, 1080), "Hop up!");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    Game game(&window);

    game.initialize();

    while (window.isOpen())
    {
        game.update();
        game.draw();
    }

    return 0;
}
