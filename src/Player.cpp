#include "Player.h"
#include "Game.h"
#include "Collision.h"

using namespace sf;

Player::Player()
{
    distance = Game::instance->WORLDSIZE + radius;
    minDistance = distance;
    JUMP_ALLOWED_DISTANCE = minDistance;

    state = STATE_RUNNING;

    boundingBox = FloatRect();
    halfSide = cos(0.25 * M_PI) * radius; // cos(45 degrees)
    boundingBox.height = halfSide * 2;
    boundingBox.width = halfSide * 2;
}

void Player::update(float dt)
{
    landSpeed += dt * ACCELERATION;

    if (Keyboard::isKeyPressed(Keyboard::Left))
        landSpeed = 0;

    angle += dt * landSpeed;

    distance += dt * verticalSpeed;

    if (!isDead)
        distance = distance < JUMP_ALLOWED_DISTANCE ? JUMP_ALLOWED_DISTANCE : distance;

    verticalSpeed -= dt * Game::instance->GRAVITY;

    if (!isDead)
        verticalSpeed = distance <= minDistance ? 0 : verticalSpeed;

    boundingBox.top = this->getPos().y - halfSide;
    boundingBox.left = this->getPos().x - halfSide;

    switch(state)
    {
        case STATE_BUILDING_UP:
            jumpBuildup += dt * buildupSpeed;
            jumpBuildup = jumpBuildup > MAX_JUMP_BUILDUP ? MAX_JUMP_BUILDUP : jumpBuildup;

            if (!Keyboard::isKeyPressed(Keyboard::Space))
            {
                state = STATE_JUMPING;
                verticalSpeed = jumpBuildup;
                jumpBuildup = MIN_JUMP_BUILDUP;
            }
            if (isDead)
                state = STATE_DEAD_WAIT;
            break;
        case STATE_JUMPING:
            if (distance <= JUMP_ALLOWED_DISTANCE)
            {
                std::cout << distance << " < " << JUMP_ALLOWED_DISTANCE << ", switching to running state\n";
                state = STATE_RUNNING;
            }
            if (isDead)
                state = STATE_DEAD_WAIT;
            break;
        case STATE_RUNNING:
            if (Keyboard::isKeyPressed(Keyboard::Space))
            {
                state = STATE_BUILDING_UP;
            }
            if (isDead)
                state = STATE_DEAD_WAIT;
            break;
        case STATE_DEAD_WAIT:
            landSpeed = 0;
            verticalSpeed = 0;

            if (Game::instance->getTotalTime().asMilliseconds() - timeOfDeath > DEATH_WAIT_TIME)
            {
                state = STATE_DEAD_FALL;
                verticalSpeed = 50;
            }
            break;
        case STATE_DEAD_FALL:

            verticalSpeed -= dt * Game::instance->GRAVITY;
            break;
        default:
            std::cout << "WARNING: PLAYER IN UNKNOWN STATE\N";
            break;
    }

    if (rainbow) {
        color = Game::instance->timeToRainbow(Game::instance->getTotalTime().asMilliseconds() * RAINBOW_MULT);
    }
}

void Player::draw(RenderWindow* window)
{
    float flattening = 1 - 0.3 * ((jumpBuildup - MIN_JUMP_BUILDUP) / (MAX_JUMP_BUILDUP + MIN_JUMP_BUILDUP));

    CircleShape player;
    player.setPointCount(128);
    player.setRadius(radius);
    player.setOrigin(Vector2f(radius / flattening, radius));
    player.setPosition(Game::instance->polarToVector(distance, angle));
    player.setFillColor(color);
    player.setScale(flattening, 1);
    player.setRotation(angle);

    if (state == STATE_DEAD_FALL || state == STATE_DEAD_WAIT)
        player.setFillColor(Color::Red);

    window->draw(player);

    //window->draw(getSprite());
}

void Player::kill(Time totalTime)
{
    if (!isDead)
        timeOfDeath = totalTime.asMilliseconds();
    isDead = true;
}

// For collissions...
Sprite Player::getSprite()
{
    Sprite sprite;
    sprite.setPosition(boundingBox.left + halfSide, boundingBox.top + halfSide);
    sprite.setScale(boundingBox.width, boundingBox.height);
    sprite.setTexture(Game::instance->textures["maze.png"]);
    sprite.setTextureRect({0, 0, 1, 1});
    sprite.setOrigin(0.5, 0.5);
    sprite.setRotation(angle);
    return sprite;
}

Vector2f Player::getPos()
{
    return Game::instance->polarToVector(distance, angle);
}

