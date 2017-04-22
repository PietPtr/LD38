#include "Player.h"
#include "Game.h"

using namespace sf;

Player::Player()
{
    distance = Game::instance->WORLDSIZE + radius - 0.3;
    minDistance = distance;
    JUMP_ALLOWED_DISTANCE = minDistance + 0.3;
    state = STATE_RUNNING;
}

void Player::update(float dt)
{
    angle += dt * landSpeed;

    distance += dt * verticalSpeed;

    verticalSpeed -= dt * Game::instance->GRAVITY;

    verticalSpeed = distance <= minDistance ? 0 : verticalSpeed;

    /*if (wasSpacePressed && Keyboard::isKeyPressed(Keyboard::Space)) {
        jumpBuildup += dt * buildupSpeed;
        jumpBuildup = jumpBuildup > MAX_JUMP_BUILDUP ? MAX_JUMP_BUILDUP : jumpBuildup;
    } else if (wasSpacePressed && distance < JUMP_ALLOWED_DISTANCE) {
        verticalSpeed = jumpBuildup;
        jumpBuildup = MIN_JUMP_BUILDUP;
    }*/

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
            break;
        case STATE_JUMPING:
            if (distance < JUMP_ALLOWED_DISTANCE && verticalSpeed < 1)
            {
                state = STATE_RUNNING;
            }
            break;
        case STATE_RUNNING:
            if (Keyboard::isKeyPressed(Keyboard::Space))
            {
                state = STATE_BUILDING_UP;
            }
            break;
    }

    std::cout << state << " " << distance << " " << JUMP_ALLOWED_DISTANCE << "\n";

    wasSpacePressed = Keyboard::isKeyPressed(Keyboard::Space);
}

void Player::draw(RenderWindow* window)
{
    float flattening = 1 - 0.3 * ((jumpBuildup - MIN_JUMP_BUILDUP) / (MAX_JUMP_BUILDUP + MIN_JUMP_BUILDUP));

    CircleShape player;
    player.setPointCount(128);
    player.setRadius(radius);
    player.setOrigin(Vector2f(radius / flattening, radius));
    player.setPosition(Game::instance->polarToVector(distance, angle));
    player.setFillColor(Color::Black);
    player.setScale(flattening, 1);
    player.setRotation(angle);
    window->draw(player);

    RectangleShape test(Vector2f(radius*2, radius*2));
    test.setFillColor(Color::Red);
    test.setOrigin(Vector2f(radius, radius));
    test.setPosition(Game::instance->polarToVector(distance, angle));
    test.setRotation(angle);
    //window->draw(test);
}

Vector2f Player::getPos()
{
    return Game::instance->polarToVector(distance, angle);
}

