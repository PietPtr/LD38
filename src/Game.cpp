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
    hudView = View(Vector2f(1280/2, 720/2), Vector2f(1280, 720));
    window->setView(view);
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);
    textures["maze1.png"].setRepeated(true);
    textures["maze2.png"].setRepeated(true);

    resetGame();

    // at startup only
    state = WARNING;

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

    players.clear();
    obstacles.clear();
    dots.clear();

    players.push_back(Player(Keyboard::Space, 0));

    totalTime = Time::Zero;
    gameTime = Time::Zero;

    state = START;

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
            if (focusedPlayer == -1)
            {
                ready = true;
            }
            if (state == START)
            {
                if (event.key.code == Keyboard::Space) {
                    state = GAME;
                }
                else
                {
                    bool addPlayer = true;
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].getKey() == event.key.code) {
                            addPlayer = false;
                        }
                    }
                    if (players.size() >= MAX_PLAYERS || std::find(FORBIDDEN_JUMP_KEYS.begin(),
                        FORBIDDEN_JUMP_KEYS.end(), event.key.code) != FORBIDDEN_JUMP_KEYS.end())
                        addPlayer = false;

                    if (addPlayer) {
                        players.push_back(Player(event.key.code, 0.88 * players.size()));
                        std::cout << "Added a player with key " << event.key.code << "\n";
                    }
                }
            }
            if (state == GAMEOVER && totalTime.asMilliseconds() - players[focusedPlayer].getTimeOfDeath() > 1200) {
                resetGame();
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

    view.setCenter(polarToVector(players[focusedPlayer].getMinDistance(), players[focusedPlayer].getAngle() + VIEW_ANGLE_OFFSET));
    view.setSize(16*5 * zoom, 9*5 * zoom);

    bool allDead = true;
    for (int i = 0; i < players.size(); i++) {
        players[i].update(dt.asSeconds());
        if (!players[i].isDead())
            focusedPlayer = i;
        allDead &= players[i].isDead();
    }

    if (state == WARNING && totalTime.asSeconds() > WARNING_TIME) {
        state = START;
    }

    if (allDead) {
        state = GAMEOVER;
    }

    if (gameTime.asSeconds() < eventTimes[EVENT_VIEW_ROTATE]) {
        view.setRotation(players[focusedPlayer].getAngle() + 90 + VIEW_ANGLE_OFFSET);
        viewRotation = view.getRotation();
    }

    if (Keyboard::isKeyPressed(Keyboard::V)) {
        view.setCenter(0, 0);
        view.setSize(1280, 720);
        view.setRotation(0);
    }
    if (Keyboard::isKeyPressed(Keyboard::R)) {
        view.setRotation(0);
        view.setCenter(players[focusedPlayer].getPos());
    }

    if (state == GAME)
    {
        int closestObstacle = 0;
        for (int i = obstacles.size() - 1; i >= 0; i--)
        {
            if ((int)fabs(obstacles[i].getAngle() - players[focusedPlayer].getAngle()) % 360 <
                (int)fabs(obstacles[closestObstacle].getAngle() - players[focusedPlayer].getAngle()) % 360)
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

        for (int i = -1; i <= 1; i++) {
            for (int p = 0; p < players.size(); p++) {
                if (Collision::BoundingBoxTest(players[p].getSprite(), obstacles[closestObstacle + i].getSprite())) {
                    players[p].kill(totalTime);
                }
            }
        }


        if (focusedPlayer == -1 && ready)
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
        }

        if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_PLAYER]) {
            for(int i = 0; i < players.size(); i++) {
                players[i].setRainbow(true);
            }
        }
    }


    frame++;

    if (frame % 60 == 0) {
        std::cout << 1 / dt.asSeconds() << " FPS\n";
    }
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

    for (int i = 0; i < players.size(); i++) {
        players[i].draw(window, players.size() > 1);
    }


    //////////////
    // HUD VIEW //
    //////////////

    window->setView(hudView);

    Text time(std::to_string(gameTime.asSeconds()), font);
    time.setCharacterSize(64);

    if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_BG])
        time.setColor(timeToRainbow((totalTime.asMilliseconds() + 1234) * WORLD_RAINDOW_MULT2));
    else
        time.setColor(Color::Black);

    time.setPosition(Vector2f(10, 0));
    window->draw(time);

    if (gameTime.asSeconds() > eventTimes[EVENT_THE_LAST_EVENT] && frame % 4 == 0) {
        Sprite l;
        l.setTexture(textures["last.png"]);
        l.setPosition(Vector2f(0, 0));
        window->draw(l);
    }

    if (state == START) {
        Text start("HOLD SPACE TO START", font);
        start.setCharacterSize(64);
        start.setOrigin(start.getLocalBounds().width / 2, 96);
        start.setPosition(1280/2, 300);
        start.setColor(Color::Black);
        window->draw(start);

        Text multi("OR PRESS ANY OTHER BUTTON TO ADD ANOTHER PLAYER", font);
        multi.setCharacterSize(16);
        multi.setOrigin(multi.getLocalBounds().width / 2, 0);
        multi.setPosition(1280/2, 270);
        multi.setColor(Color::Black);
        window->draw(multi);
    }

    if (state == WARNING) {
        float opacity = (- ((0.5 * WARNING_TIME) / WARNING_TIME)) * totalTime.asSeconds() + WARNING_TIME / 2;

        opacity = opacity > 1 ? 1 : opacity;

        Sprite black;
        black.setTexture(textures["px.png"]);
        black.setScale(1280, 720);
        black.setColor(Color(0, 0, 0, opacity * 255));
        window->draw(black);

        Text warn("EPILEPSY WARNING", font);
        warn.setCharacterSize(128);
        warn.setOrigin(warn.getLocalBounds().width / 2, warn.getLocalBounds().height / 2);
        warn.setPosition(1280/2, 100);
        warn.setColor(Color(255, 255, 255, opacity * 255));
        window->draw(warn);
    }

    if (state == GAMEOVER) {
        Color overColor = Color::Black;
        if (gameTime.asSeconds() > eventTimes[EVENT_RAINBOW_BG])
            overColor = timeToRainbow(totalTime.asMilliseconds() * 5);

        Text over("GAME OVER", font);
        over.setCharacterSize(128);
        over.setOrigin(over.getLocalBounds().width / 2, 0);
        over.setPosition(1280/2, 0);
        over.setColor(overColor);
        window->draw(over);


        if (players.size() > 1) {
            std::string winStr = "";
            winStr += "PLAYER ";
            winStr += Game::getKeyName(players[focusedPlayer].getKey());
            winStr += " WON!";
            Text win(winStr, font);
            win.setCharacterSize(64);
            win.setOrigin(win.getLocalBounds().width / 2, 0);
            win.setPosition(1280/2, 180);
            win.setColor(overColor);
            window->draw(win);
        }

        std::string scoreStr = "SCORE: " + std::to_string(gameTime.asSeconds()) + " SECONDS";
        Text score(scoreStr, font);
        score.setCharacterSize(64);
        score.setOrigin(score.getLocalBounds().width / 2, 0);
        score.setPosition(1280/2, 120);
        score.setColor(overColor);
        window->draw(score);

        Text restart("PRESS ANY KEY TO CONTINUE", font);
        restart.setCharacterSize(32);
        restart.setOrigin(restart.getLocalBounds().width / 2, 0);
        restart.setPosition(1280/2, 300);
        if ((int)(totalTime.asSeconds() * 3) % 2 == 0 && gameTime.asSeconds() < eventTimes[EVENT_RAINBOW_BG])
            restart.setColor(Color::White);
        else
            restart.setColor(overColor);
        window->draw(restart);

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
    if (state == GAME)
        gameTime += dt;
}

std::string Game::getKeyName(sf::Keyboard::Key key) {
    switch(key) {
        default:
        case sf::Keyboard::Unknown:
                return "Unknown";
        case sf::Keyboard::A:
                return "A";
        case sf::Keyboard::B:
                return "B";
        case sf::Keyboard::C:
                return "C";
        case sf::Keyboard::D:
                return "D";
        case sf::Keyboard::E:
                return "E";
        case sf::Keyboard::F:
                return "F";
        case sf::Keyboard::G:
                return "G";
        case sf::Keyboard::H:
                return "H";
        case sf::Keyboard::I:
                return "I";
        case sf::Keyboard::J:
                return "J";
        case sf::Keyboard::K:
                return "K";
        case sf::Keyboard::L:
                return "L";
        case sf::Keyboard::M:
                return "M";
        case sf::Keyboard::N:
                return "N";
        case sf::Keyboard::O:
                return "O";
        case sf::Keyboard::P:
                return "P";
        case sf::Keyboard::Q:
                return "Q";
        case sf::Keyboard::R:
                return "R";
        case sf::Keyboard::S:
                return "S";
        case sf::Keyboard::T:
                return "T";
        case sf::Keyboard::U:
                return "U";
        case sf::Keyboard::V:
                return "V";
        case sf::Keyboard::W:
                return "W";
        case sf::Keyboard::X:
                return "X";
        case sf::Keyboard::Y:
                return "Y";
        case sf::Keyboard::Z:
                return "Z";
        case sf::Keyboard::Num0:
                return "Num0";
        case sf::Keyboard::Num1:
                return "Num1";
        case sf::Keyboard::Num2:
                return "Num2";
        case sf::Keyboard::Num3:
                return "Num3";
        case sf::Keyboard::Num4:
                return "Num4";
        case sf::Keyboard::Num5:
                return "Num5";
        case sf::Keyboard::Num6:
                return "Num6";
        case sf::Keyboard::Num7:
                return "Num7";
        case sf::Keyboard::Num8:
                return "Num8";
        case sf::Keyboard::Num9:
                return "Num9";
        case sf::Keyboard::Escape:
                return "Escape";
        case sf::Keyboard::LControl:
                return "LControl";
        case sf::Keyboard::LShift:
                return "LShift";
        case sf::Keyboard::LAlt:
                return "LAlt";
        case sf::Keyboard::LSystem:
                return "LSystem";
        case sf::Keyboard::RControl:
                return "RControl";
        case sf::Keyboard::RShift:
                return "RShift";
        case sf::Keyboard::RAlt:
                return "RAlt";
        case sf::Keyboard::RSystem:
                return "RSystem";
        case sf::Keyboard::Menu:
                return "Menu";
        case sf::Keyboard::LBracket:
                return "LBracket";
        case sf::Keyboard::RBracket:
                return "RBracket";
        case sf::Keyboard::SemiColon:
                return "SemiColon";
        case sf::Keyboard::Comma:
                return "Comma";
        case sf::Keyboard::Period:
                return "Period";
        case sf::Keyboard::Quote:
                return "Quote";
        case sf::Keyboard::Slash:
                return "Slash";
        case sf::Keyboard::BackSlash:
                return "BackSlash";
        case sf::Keyboard::Tilde:
                return "Tilde";
        case sf::Keyboard::Equal:
                return "Equal";
        case sf::Keyboard::Dash:
                return "Dash";
        case sf::Keyboard::Space:
                return "Space";
        case sf::Keyboard::Return:
                return "Return";
        case sf::Keyboard::BackSpace:
                return "BackSpace";
        case sf::Keyboard::Tab:
                return "Tab";
        case sf::Keyboard::PageUp:
                return "PageUp";
        case sf::Keyboard::PageDown:
                return "PageDown";
        case sf::Keyboard::End:
                return "End";
        case sf::Keyboard::Home:
                return "Home";
        case sf::Keyboard::Insert:
                return "Insert";
        case sf::Keyboard::Delete:
                return "Delete";
        case sf::Keyboard::Add:
                return "Add";
        case sf::Keyboard::Subtract:
                return "Subtract";
        case sf::Keyboard::Multiply:
                return "Multiply";
        case sf::Keyboard::Divide:
                return "Divide";
        case sf::Keyboard::Left:
                return "Left";
        case sf::Keyboard::Right:
                return "Right";
        case sf::Keyboard::Up:
                return "Up";
        case sf::Keyboard::Down:
                return "Down";
        case sf::Keyboard::Numpad0:
                return "Numpad0";
        case sf::Keyboard::Numpad1:
                return "Numpad1";
        case sf::Keyboard::Numpad2:
                return "Numpad2";
        case sf::Keyboard::Numpad3:
                return "Numpad3";
        case sf::Keyboard::Numpad4:
                return "Numpad4";
        case sf::Keyboard::Numpad5:
                return "Numpad5";
        case sf::Keyboard::Numpad6:
                return "Numpad6";
        case sf::Keyboard::Numpad7:
                return "Numpad7";
        case sf::Keyboard::Numpad8:
                return "Numpad8";
        case sf::Keyboard::Numpad9:
                return "Numpad9";
        case sf::Keyboard::F1:
                return "F1";
        case sf::Keyboard::F2:
                return "F2";
        case sf::Keyboard::F3:
                return "F3";
        case sf::Keyboard::F4:
                return "F4";
        case sf::Keyboard::F5:
                return "F5";
        case sf::Keyboard::F6:
                return "F6";
        case sf::Keyboard::F7:
                return "F7";
        case sf::Keyboard::F8:
                return "F8";
        case sf::Keyboard::F9:
                return "F9";
        case sf::Keyboard::F10:
                return "F10";
        case sf::Keyboard::F11:
                return "F11";
        case sf::Keyboard::F12:
                return "F12";
        case sf::Keyboard::F13:
                return "F13";
        case sf::Keyboard::F14:
                return "F14";
        case sf::Keyboard::F15:
                return "F15";
        case sf::Keyboard::Pause:
                return "Pause";
    }
}
