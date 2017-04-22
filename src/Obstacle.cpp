#include "Obstacle.h"
#include "Game.h"

using namespace sf;

// Generates a rectangle with given properties
// Properties not given are randomized
Obstacle::Obstacle(float angle)
{
    this->angle = angle;

    this->distance = Game::instance->randint(
    Game::instance->WORLDSIZE - MIN_WORLDSIZE_DEVIATION,
    Game::instance->WORLDSIZE + MAX_WORLDSIZE_DEVIATION);

    this->rotation = Game::instance->randint(0, 360);

    rect.width = Game::instance->randint(MIN_SIZE, MAX_SIZE);
    rect.height = Game::instance->randint(MIN_SIZE, MAX_SIZE);
    rect.left = getPos().x;
    rect.top = getPos().y;

    while (isValidObstacle() < 1 && isValidObstacle() >= 3)
    {
        int in = isValidObstacle();

        if (in < 1) {
            this->distance -= 0.1;
        } else if (in >= 3) {
            this->distance += 0.1;
        }
    }
}

void Obstacle::update(float dt)
{

}

void Obstacle::draw(RenderWindow* window)
{
    window->draw(getSprite());
}

int Obstacle::isValidObstacle()
{
    Transform tf;
    tf.rotate(rotation);
    std::vector<Vector2f> points;
    points.push_back(tf.transformPoint(Vector2f(rect.left             , rect.top              )));
    points.push_back(tf.transformPoint(Vector2f(rect.left             , rect.top + rect.height)));
    points.push_back(tf.transformPoint(Vector2f(rect.left + rect.width, rect.top              )));
    points.push_back(tf.transformPoint(Vector2f(rect.left + rect.width, rect.top + rect.height)));

    int inCount = 0;
    for (int i = 0; i < points.size(); i++)
    {
        // distance to the center of the world
        float distance = sqrt(pow(points[i].x, 2) + pow(points[i].y, 2));

        if (distance < Game::instance->WORLDSIZE) {
            inCount++;
        }
    }

    std::cout << inCount << "\n";

    return inCount;
}

Sprite Obstacle::getSprite()
{
    Sprite sprite;
    sprite.setPosition(getPos());
    sprite.setPosition(rect.left, rect.top);
    sprite.setScale(rect.width, rect.height);
    sprite.setTexture(Game::instance->textures["px.png"]);
    sprite.setColor(Color::Black);
    sprite.setTextureRect({0, 0, 1, 1});
    sprite.setOrigin(0.5, 0.5);
    sprite.setRotation(rotation);

    return sprite;
}

Vector2f Obstacle::getPos()
{
    return Game::instance->polarToVector(distance, angle);
}
