#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"
#include "Player.h"

using namespace sf;

class Player;

class Game
{
    public:
        const double PI = 3.14159265359;
        const float WORLDSIZE = 260;
        const float VIEW_ANGLE_OFFSET = 5;
        const float GRAVITY = 98.1;

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
    protected:
    private:
        RenderWindow* window;
        View view;

        Time dt;
        Time totalTime;
        Clock clock;
        int frame = 0;
        bool focus = false;

        int windowWidth = 1280;
        int windowHeight = 720;

        std::vector<std::string> audioFileNames {  };
        std::vector<std::string> textureFileNames {  };

        std::map<std::string, Audio*> sfx;
        std::map<std::string, Texture> textures;

        Player* player = nullptr;
};


#endif // GAME_H
