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
        Player(Keyboard::Key jumpKey, float angle);
        void update(float dt);
        void draw(RenderWindow* window, bool drawKey);

        Vector2f getPos();
        float getAngle() { return angle; }
        float getDistance() { return distance; }
        float getMinDistance() { return minDistance; }
        Sprite getSprite();

        void kill(Time totalTime);
        bool isDead() { return dead; }

        void setRainbow(bool r) { rainbow = r; }

        Keyboard::Key getKey() { return jumpKey; }
        long getTimeOfDeath() { return timeOfDeath; }
    protected:
    private:
        const float MAX_JUMP_BUILDUP = 55.0;
        const float MIN_JUMP_BUILDUP = 15.0;
        const float ACCELERATION = 0.04;
        const int DEATH_WAIT_TIME = 100;
        const float RAINBOW_MULT = 1.2;
        float JUMP_ALLOWED_DISTANCE = 0.0;        // units
        float halfSide = 0.0;                     // units
        float radius = 2.0;                       // units
        float angle = 0.0;                        // degrees
        float distance = 0.0;                     // units from the center
        float landSpeed = 7.5;                    // degrees per second
        float verticalSpeed = 0.0;                // units per second
        float minDistance = 0.0;                  // units
        float jumpBuildup = MIN_JUMP_BUILDUP;
        float buildupSpeed = 150.0;

        State state;

        bool dead = false;

        FloatRect boundingBox;

        long timeOfDeath = -1;

        Color color = Color::Black;

        bool rainbow = false;

        Keyboard::Key jumpKey;
};

#endif // PLAYER_H
