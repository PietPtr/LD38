#ifndef DOT_H
#define DOT_H
#include <SFML/Graphics.hpp>

using namespace sf;

enum DotState {
    DOT_OFF,
    DOT_ON,
    DOT_MOVING,
    DOT_RAINBOW
};

class Dot
{
    public:
        Dot(float distance, float angle);

        void update(float dt);
        void draw(RenderWindow* window);

        void setState(DotState state) { this->state = state; }
        void turnOn();

        void setColor(Color c) { color = c; }

    protected:
    private:
        const float MAX_RADIUS = 0.1;
        const float MOVE_DISTANCE = 0.2;
        const float MOVE_SPEED = 3;
        const float RADIUS_GROW = 0.0;
        const float SEC_MAX_RADIUS = 0.3;

        long turnOnTime = 0;
        Color color = Color::Black;
        float radius = 0.0;
        float angle = 0.0;
        float distance = 0.0;
        float baseDistance = 0.0;
        DotState state = DOT_OFF;
};

#endif // DOT_H
