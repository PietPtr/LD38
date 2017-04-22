#include "Game.h"
#include "Dot.h"
#include <SFML/Graphics.hpp>

using namespace sf;

Dot::Dot(float distance, float angle)
{
    this->distance = distance;
    this->angle = angle;
    baseDistance = this->distance;
}

void Dot::update(float totalTime)
{
    if (state == DOT_OFF) {
        return;
    }
    if (state == DOT_ON) {
        radius = (Game::instance->getTotalTime().asMicroseconds() - turnOnTime) / 5000000.0;
        if (radius > MAX_RADIUS)
            radius = MAX_RADIUS;
    }
    if (state == DOT_MOVING || state == DOT_RAINBOW) {
        distance = baseDistance + MOVE_DISTANCE * (cos(totalTime * MOVE_SPEED + turnOnTime));
    }
    if (state == DOT_RAINBOW) {
        color = Game::instance->timeToRainbow(Game::instance->getTotalTime().asMilliseconds() + turnOnTime);
        radius += RADIUS_GROW;
        if (radius > SEC_MAX_RADIUS)
            radius = SEC_MAX_RADIUS;
    }
}

void Dot::draw(RenderWindow* window)
{
    CircleShape dot;
    dot.setPosition(Game::instance->polarToVector(distance, angle));
    dot.setRadius(radius);
    dot.setOrigin(radius, radius);
    dot.setFillColor(color);

    window->draw(dot);
}

void Dot::turnOn()
{
    if (state == DOT_OFF)
    {
        state = DOT_ON;
        turnOnTime = Game::instance->getTotalTime().asMicroseconds();
    }
}
