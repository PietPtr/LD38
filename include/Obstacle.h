#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Obstacle
{
    public:
        Obstacle(float angle);

        void update(float dt);
        void draw(RenderWindow* window);

        Sprite getSprite();
        FloatRect getRect() { return rect; }
        Vector2f getPos();
        float getAngle() { return angle; }
        int isValidObstacle();
    protected:
    private:
        // Randomize settings
        const float MIN_WORLDSIZE_DEVIATION = 1;
        const float MAX_WORLDSIZE_DEVIATION = 2;
        const float MAX_SIZE = 12;
        const float MIN_SIZE = 5;
        // ----

        float angle = 0.0;
        float distance = 0.0;
        float rotation = 0.0;

        FloatRect rect;
};

#endif // OBSTACLE_H
