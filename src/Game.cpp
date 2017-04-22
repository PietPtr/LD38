#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Collision.h"

using namespace sf;

Game* Game::instance;

Game::Game(RenderWindow* _window)
{
    Game::instance = this;
    window = _window;

    view = View(Vector2f(0, 0), Vector2f(1280, 720));
    hudView = View(Vector2f(0, 0), Vector2f(1280, 720));
    window->setView(view);
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);
    textures["maze1.png"].setRepeated(true);
    textures["maze2.png"].setRepeated(true);

    resetGame();

    font.loadFromFile("font/TheJewishBitmap.ttf");
}

void Game::resetGame()
{
    ready = false;

    zoomDirection = 1;
    zoomSpeed = 0.001;
    zoomAcceleration = 0.00005;
    zoom = 1;

    viewRotation = 0.0;
    extraViewRotation = 0.0;
    viewRotationSpeed = 0.0;
    viewRotationAcceleration = 1.0;

    player = new Player();
    obstacles.clear();
    dots.clear();

    totalTime = Time::Zero;

    for (int i = 20; i < 360; i += DEGREES_PER_OBSTACLE)
    {
        restartClock();
        int deviation = randint(-OBSTACLE_POS_DEVIATION, OBSTACLE_POS_DEVIATION);
        obstacles.push_back(Obstacle(i + deviation));
        obstacles.push_back(Obstacle(i + deviation));
    }

    for (int i = 0; i < 1800; i++)
    {
        restartClock();
        dots.push_back(Dot(WORLDSIZE + randint(0, 40), i + (randint(-10, 10) / 10.0)));
    }
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
            if (player->isDead())
            {
                ready = true;
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
    view.setSize(16*5 * zoom, 9*5 * zoom);

    if (gameTime.asSeconds() < eventTimes[EVENT_VIEW_ROTATE]) {
        view.setRotation(player->getAngle() + 90 + VIEW_ANGLE_OFFSET);
        viewRotation = view.getRotation();
    }

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

        if (gameTime.asSeconds() > eventTimes[EVENT_PULSATING_OBSTACLES])
        {
            obstacles[i].setState(OBSTACLE_PULSE);
        }

        if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_OBSTACLES])
        {
            obstacles[i].setState(OBSTACLE_RAINBOW);
        }

        obstacles[i].update(totalTime.asSeconds());
    }

    if (Collision::BoundingBoxTest(player->getSprite(), obstacles[closestObstacle].getSprite())) {
        player->kill(totalTime);
    }

    if (player->getDistance() < WORLDSIZE - 260 && ready)
    {
        resetGame();
    }

    for (int i = 0; i < dots.size(); i++)
    {
        dots[i].update(totalTime.asSeconds());

        if (gameTime.asSeconds() > eventTimes[EVENT_BACKGROUND] && randint(0, 100) > 99)
            dots[i].turnOn();
        if (gameTime.asSeconds() > eventTimes[EVENT_MOVING_BACKGROUND])
            dots[i].setState(DOT_MOVING);
        if (gameTime.asSeconds() > eventTimes[EVENT_MOVING_BACKGROUND]) {
            dots[i].setState(DOT_RAINBOW);
        }
    }

    if (gameTime.asSeconds() > eventTimes[EVENT_VIEW_ZOOM])
    {
        if (zoomSpeed < MAX_ZOOM_SPEED)
            zoomSpeed += zoomAcceleration * dt.asSeconds();
        zoom += zoomSpeed * zoomDirection;
        if (zoom > MAX_ZOOM)
            zoomDirection = -1;
        if (zoom < MIN_ZOOM)
            zoomDirection = 1;
    }

    if (gameTime.asSeconds() > eventTimes[EVENT_VIEW_ROTATE])
    {
        viewRotation += viewRotationSpeed * dt.asSeconds();
        viewRotationSpeed += viewRotationAcceleration * dt.asSeconds();
        view.setRotation(viewRotation);
        std::cout << viewRotationSpeed << "\n";
    }

    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_PLAYER]) {
        player->setRainbow(true);
    }

    frame++;
}

void Game::draw()
{
    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_BG])
        window->clear(timeToRainbow(totalTime.asMilliseconds()));
    else
        window->clear(Color::White);

    window->setView(view);

    for (int i = 0; i < dots.size(); i++)
    {
        dots[i].draw(window);
    }

    for (int i = 0; i < obstacles.size(); i++)
    {
        obstacles[i].draw(window);
    }

    CircleShape world = CircleShape(WORLDSIZE, 1024);
    world.setOrigin(Vector2f(WORLDSIZE, WORLDSIZE));
    world.setTexture(&textures["maze1.png"]);
    world.setTextureRect({ 0, 0, 200, 200 });
    world.setOutlineColor(Color(0,0,0));
    world.setOutlineThickness(0);

    world.setFillColor(Color(7, 7, 7));
    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_WORLD])
        world.setFillColor(timeToRainbow(totalTime.asMilliseconds() * WORLD_RAINDOW_MULT));
    window->draw(world);

    world.setTexture(&textures["maze2.png"]);
    world.setFillColor(Color(0,0,0));
    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_WORLD])
        world.setFillColor(timeToRainbow(totalTime.asMilliseconds() * WORLD_RAINDOW_MULT2));
    window->draw(world);

    player->draw(window);

    window->setView(hudView);

    Text time(std::to_string(gameTime.asSeconds()), font);
    time.setCharacterSize(64);

    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_BG])
        time.setColor(timeToRainbow(totalTime.asMilliseconds() * WORLD_RAINDOW_MULT2));
    else
        time.setColor(Color::Black);

    time.setPosition(Vector2f(-630, -360));
    window->draw(time);

    if (gameTime.asSeconds() > eventTimes[EVENT_THE_LAST_EVENT] && frame % 4 == 0) {
        Sprite l;
        l.setTexture(textures["last.png"]);
        l.setPosition(Vector2f(-640, -360));
        window->draw(l);
    }

    window->display();
}

Color Game::timeToRainbow(long millis)
{
    float value = millis / 200.0;

    Color color;
    color.r = sin(value + 0) * 255;
    color.g = sin(value + 2) * 255;
    color.b = sin(value + 4) * 255;

    return color;
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
    if (!player->isDead())
        gameTime = totalTime;
}
