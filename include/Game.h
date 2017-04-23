#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"
#include "Player.h"
#include "Obstacle.h"
#include "Dot.h"

using namespace sf;

enum Events {
    EVENT_BACKGROUND,               // Black twinkeling dots appear in the background
    EVENT_PULSATING_OBSTACLES,      // Obstacles start changing in size slightly
    EVENT_VIEW_ZOOM,                // The view starts to slowly zoom out and back in
    EVENT_MOVING_BACKGROUND,        // Background dots start moving in circles
    EVENT_VIEW_ROTATE,              // The view is not stationary with the player anymore and slowly starts to rotate
    EVENT_RAINBOW_BG_DOTS,          // Twinkeling dots become rainbow colored
    EVENT_RAINBOW_OBSTACLES,        // Obstacles get random rainbow colors
    EVENT_RAINBOW_WORLD,            // World gets rainbow colors
    EVENT_RAINBOW_BG,               // Background itself gets rainbow colors
    EVENT_RAINBOW_PLAYER,           // Player gets rainbow colors
    EVENT_THE_LAST_EVENT
};

enum GameState {
    WARNING,
    START,
    GAME,
    GAMEOVER
};

class Game
{
    public:
        const double PI = 3.14159265359;
        const float WARNING_TIME = 3;
        const float WORLDSIZE = 260;
        const float VIEW_ANGLE_OFFSET = 5;
        const float GRAVITY = 98.1;
        const float DEGREES_PER_OBSTACLE = 18;
        const float OBSTACLE_POS_DEVIATION = 2;
        const float NEXT_EVENT_TIME = 9.81;
        const float MAX_ZOOM = 1.3;
        const float MIN_ZOOM = 0.8;
        const float MAX_ZOOM_SPEED = 0.1;
        const float WORLD_RAINDOW_MULT = 0.5;
        const float WORLD_RAINDOW_MULT2 = 0.6;
        const float MAX_PLAYERS = 5;
        const std::vector<Keyboard::Key> FORBIDDEN_JUMP_KEYS { Keyboard::F1 };

        Game(RenderWindow* window);
        static Game* instance;
        void initialize();
        void update();
        void draw();
        bool isWindowOpen();

        void loadAudio(std::vector<std::string> fileNames);
        void loadTextures(std::vector<std::string> fileNames);

        int randint(int low, int high, int seed);
        int randint(int low, int high);

        void takeScreenshot();

        Vector2f polarToVector(float distance, float angle);

        Color timeToRainbow(long millis);

        void restartClock();
        Time getTotalTime() { return totalTime; }

        std::map<std::string, Audio*> sfx;
        std::map<std::string, Texture> textures;
        Font font;

        void resetGame();

        GameState getGameState() { return state; }

        static std::string getKeyName(sf::Keyboard::Key key);
    protected:
    private:
        RenderWindow* window;
        View view;
        View hudView;

        GameState state = WARNING;

        Time dt;
        Time totalTime;
        Time gameTime;
        Clock clock;
        int frame = 0;
        bool focus = false;

        int windowWidth = 1280;
        int windowHeight = 720;

        std::vector<std::string> audioFileNames {  };
        std::vector<std::string> textureFileNames { "maze1.png", "maze2.png", "px.png", "last.png" };


        //Player* player = nullptr;
        std::vector<Player> players;
        int focusedPlayer = 0;

        std::vector<Obstacle> obstacles;
        std::vector<Dot> dots;

        std::map<Events, int> eventTimes =
        {
            {EVENT_BACKGROUND,          1 * NEXT_EVENT_TIME},
            {EVENT_PULSATING_OBSTACLES, 2 * NEXT_EVENT_TIME},
            {EVENT_VIEW_ZOOM,           3 * NEXT_EVENT_TIME},
            {EVENT_MOVING_BACKGROUND,   4 * NEXT_EVENT_TIME},
            {EVENT_VIEW_ROTATE,         5 * NEXT_EVENT_TIME},
            {EVENT_RAINBOW_BG_DOTS,     6 * NEXT_EVENT_TIME},
            {EVENT_RAINBOW_BG,          7 * NEXT_EVENT_TIME},
            {EVENT_RAINBOW_OBSTACLES,   8 * NEXT_EVENT_TIME},
            {EVENT_RAINBOW_WORLD,       9 * NEXT_EVENT_TIME},
            {EVENT_RAINBOW_PLAYER,     10 * NEXT_EVENT_TIME},
            {EVENT_THE_LAST_EVENT,     11 * NEXT_EVENT_TIME},
        };

        float zoomDirection = 1;
        float zoomSpeed = 0.001;
        float zoomAcceleration = 0.00005;
        float zoom = 1;

        float viewRotation = 0.0;
        float extraViewRotation = 0.0;
        float viewRotationSpeed = 0.0;
        float viewRotationAcceleration = 1.0;

        bool ready = false;
};


#endif // GAME_H
