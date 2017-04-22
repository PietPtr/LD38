#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>

using namespace sf;

enum State
{
    STATE_BUILDING_UP,
    STATE_JUMPING,
    STATE_RUNNING
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
    protected:
    private:
        const float MAX_JUMP_BUILDUP = 55.0;
        const float MIN_JUMP_BUILDUP = 15.0;
        float JUMP_ALLOWED_DISTANCE = 0.0;        // units
        float radius = 2.0;                       // units
        float angle = 0.0;                        // degrees
        float distance = 0.0;                     // units from the center
        float landSpeed = 4.0;                    // degrees per second
        float verticalSpeed = 0.0;                // units per second
        float minDistance = 0.0;                  // units
        float jumpBuildup = MIN_JUMP_BUILDUP;
        float buildupSpeed = 150.0;

        State state;

        bool wasSpacePressed = false;

};

#endif // PLAYER_H
