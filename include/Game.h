#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"

using namespace sf;

class Game
{
    public:
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
    protected:
    private:
        RenderWindow* window;

        Time dt;
        Time totalTime;
        Clock clock;
        int frame = 0;
        bool focus = false;

        int windowWidth = 1280;
        int windowHeight = 720;

        std::vector<std::string> audioFileNames { "test.wav" };
        std::vector<std::string> textureFileNames { "test.png" };

        std::map<std::string, Audio*> sfx;
        std::map<std::string, Texture> textures;
};
