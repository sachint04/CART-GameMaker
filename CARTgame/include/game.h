#pragma once
#include <string>
#include "Application.h"

#define REPLAY_ANIM replay_anim
//using namespace std;


#define __LOGGER__

using namespace std::chrono;

namespace cart {

    class PuzzleImageController;
    class HUD;
    class Game :public Application 
    {
       
    public:

        Game(int _winWidth, int _winHeight, const std::string& title);
        void Init() override;
        std::string& GetAssetsPath()override;
        std::string GetResourceDisplayPath()override;
        float GetIconSize()override;
        ~Game();
    

    private:
        Logger* logger;

    };



   
}