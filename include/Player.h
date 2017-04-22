#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>

using namespace sf;

enum State
{
    STATE_BUILDING_UP,
    STATE_JUMPING,
    STATE_RUNNING,
    STATE_DEAD_WAIT,
    STATE_DEAD_FALL
};

class Player
{
    public:
        Player();
        void update(float dt);
        void draw(RenderWindow* window);

        Vector2f getPos();
        float getAngle() { return angle; }
        float getDistance() { return distance; }
        float getMinDistance() { return minDistance; }
        Sprite getSprite();

        void kill(Time totalTime);
    protected:
    private:
        const float MAX_JUMP_BUILDUP = 55.0;
        const float MIN_JUMP_BUILDUP = 15.0;
        const float ACCELERATION = 0.01;
        const int DEATH_WAIT_TIME = 100;
        float JUMP_ALLOWED_DISTANCE = 0.0;        // units
        float halfSide = 0.0;                     // units
        float radius = 2.0;                       // units
        float angle = 0.0;                        // degrees
        float distance = 0.0;                     // units from the center
        float landSpeed = 6.5;                    // degrees per second
        float verticalSpeed = 0.0;                // units per second
        float minDistance = 0.0;                  // units
        float jumpBuildup = MIN_JUMP_BUILDUP;
        float buildupSpeed = 150.0;

        State state;

        bool isDead = false;

        FloatRect boundingBox;

        long timeOfDeath;

};

#endif // PLAYER_H
