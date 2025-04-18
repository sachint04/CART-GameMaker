#pragma once
#include <chrono>
#include <iostream>
#include <vector>
#include <string>

#include <raylib.h>

#include "Application.h"
#include "Core.h"
#include "Object.h"
#include "Delegate.h"
#include "Actor.h"
#include "HUD.h"
#include "UIButton.h"
#include "Clock.h"
#include "UIElement.h"
#include "Types.h"
#include "World.h"
#include "Sprite.h"

#define REPLAY_ANIM replay_anim
//using namespace std;

using namespace std::chrono;

namespace cart {

 
    class Game :public Application 
    {
       
    public:
        APP_STATE m_APP_STATE;
        Game(int _winWidth, int _winHeight, const std::string& title);
        void Init() override;
        void Run() override;
        void BeginPlay() override;
        bool isAssetLoaded() { return m_assetsLoaded; };
        ~Game();
    protected:
        void Update(float deltaTime) override;
        void Draw(float deltaTime) override;

    private:
        int m_framesCounter;
        bool m_assetsLoaded;
       
        bool m_gameover;
        Vector2 m_locMouseDown;
          
        bool m_touch;
        bool m_debug;
        Font m_font_framd;     
        Vector2 m_locmouse;
        int m_defaultWidth;
        int m_defaultHeight;

        void CleanDebugData();
    
        
        weak<HUD> m_GameplayHUD;
        List<DebugData> m_DebugData;
    };



   
}