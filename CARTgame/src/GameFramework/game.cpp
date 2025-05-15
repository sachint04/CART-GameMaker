#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include "game.h"
#include "AssetManager.h"
#include "MathUtility.h"
#include "TextUtils.h"
#include "Core.h"
#include "Object.h"
#include "HUD.h"
#include "Text.h"
#include "World.h"
#include "Sprite.h"
#include "Shape.h"
#include "gameplayHUD.h"
#include "component/controls/transformCntrl.h"
#include "puzzleimagecontroller.h"
//using namespace std;
using namespace cart;

float APP_SCALE;
int FONT_SIZE;
std::string FONT_NAME = "fonts/framd.ttf";
namespace cart {

    // Constructor
    Game::Game(int _winWidth, int _winHeight, const std::string& title)
        : Application{ _winWidth ,_winHeight, title },
        m_framesCounter(0),
        m_assetsLoaded(false),
        m_debug{ false },
        m_DebugData{},
        m_touch{ false },
        m_locMouseDown{},
        m_gameover{ false },
        m_font_framd{},
        m_locmouse{},
        m_APP_STATE{ TITLE },
        m_defaultWidth(_winWidth),
        m_defaultHeight(_winHeight)
        
    {
      
       
    }

#pragma region App Initialization

    void Game::Init() {
        Application::Init();
        AssetManager::Get().SetAssetRootDirectory("assets");// SET ASSETS FOLDER
      
        // CREATE NEW WORLD
        weak<World> newWorld = LoadWorld<World>();
        m_CurrentWorld->Init(); // INIT WORLD

        // SET APP SCALE
        APP_SCALE = (m_winWidth < m_winHeight) ? (float)m_winWidth / (float)m_winHeight : (float)m_winHeight / (float)m_winWidth;
        FONT_SIZE = 24 * APP_SCALE;


#pragma region Draw Engine logo  
      /*  std::string uiID = "welcome";
        UI_Properties uiprop = {};
        uiprop.color = {255};
        Vector2 uiloc = { m_winWidth / 2.f - 171.f, m_winHeight / 2.f - 50.f };
        uiprop.location = uiloc;
        uiprop.size = { 342.f, 100.f };
        uiprop.rotation =  0;
        uiprop.scale = 1.f;
        uiprop.pivot = { 0,0 };
        uiprop.texture = "cartengine.png";
       weak<UIElement> ui  =  m_CurrentWorld->SpawnActor<UIElement>(uiID);
       ui.lock()->Init();
       ui.lock()->SetUIProperties(uiprop);

      std::string txtid = "welcomeid";
       Text_Properties* txtprop = new Text_Properties{};
       txtprop->color = BLUE;
       txtprop->align = ALIGN::CENTER;
       txtprop->fontsize = 30.f;
       txtprop->text = "Welcome to";
       txtprop->size = {200.f, 50.f };
       txtprop->location = { 71,  -50.f };
       txtprop->textbackground = {0};
    
       
       ui.lock()->AddText(txtid, *txtprop);
       ui.lock()->SetVisible(true);
       ui.lock()->SetActive(true);*/

     
       
    
#pragma endregion

#pragma region Create HUD
       std::string hudid = "hud-main";
       m_GameplayHUD = m_CurrentWorld->SpawnHUD<GameplayHUD>(hudid);
#pragma endregion
        int scrW = GetScreenWidth();
      int scrH = GetScreenHeight();
       weak<PuzzleImageController> imagecontrols = m_CurrentWorld->SpawnActor<PuzzleImageController>(std::string{ "control" }, std::string{ "cartengine.png" }, scrW, scrH, 300, 300, 400, 400, 300, 300);
       m_imagecontrols = imagecontrols.lock();
       m_imagecontrols.get()->Init();
       m_imagecontrols.get()->SetVisible(true);
       //Rectangle rect = ui.lock()->GetBounds();


      // delete txtprop;
       m_assetsLoaded = true;
    }

    void Game::BeginPlay()
    {      
        Application::BeginPlay();
    }
#pragma endregion

#pragma region GAME LOOP
    void Game::Run() {
        Application::Run();
    }
    void Game::Update(float deltaTime) {
        if (m_assetsLoaded == false)return;

        Application::Update(deltaTime);
        if (m_APP_STATE == TITLE) {
        }
        else if (m_APP_STATE == LEVEL_SELECTION) {
        }
        else if (m_APP_STATE == GAME) {
            if (m_gameover == true) {
            }
            else {
                Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };
                if (m_GameplayHUD.lock()->IsMouseOverUI(tPos) == false) {
                    if (IsMouseButtonPressed(0)) {

                        m_touch = true;
                    }



                    if (IsMouseButtonReleased(0)) {

                        if (m_touch == false)return;


                        m_touch = false;
                    }
                }

            }
        }

       
    }

    void Game::Draw(float deltaTime)
    {
       

      //  DrawRectangle(0, 0, m_winWidth, m_winHeight, ORANGE);
        Application::Draw(deltaTime);

       
        if (m_APP_STATE == TITLE) {

        }
        else if (m_APP_STATE == LEVEL_SELECTION) {

        }
        else if (m_APP_STATE == GAME)
        {
            // Card Rectangle
            if (m_gameover == true) {


            }
            else {


            }

        }
/*


        if (m_debug == true) {
            if (m_DebugData.size() > 0) {

                for (size_t i = 0; i < m_DebugData.size(); i++)
                {
                    switch (m_DebugData[i].shape) {
                    case P_CIRCLE:
                        DrawCircleV(m_DebugData[i].start, m_DebugData[i].radius, m_DebugData[i].color);
                        break;
                    case P_LINE:
                        DrawLineEx(m_DebugData[i].start, m_DebugData[i].end, m_DebugData[i].linewidth, m_DebugData[i].color);
                        break;
                    }
                }
                CleanDebugData();
            }
        }
        */
       
    }

#pragma endregion

#pragma region CLEAN UP
    void Game::CleanDebugData() {

        size_t linesize = m_DebugData.size();
        if (linesize == 0)return;

        m_DebugData.clear();

    }


    Game::~Game() {

        hud.reset();

    }


#pragma endregion

}

cart::Application* GetApplication()
{
    return new cart::Game{800, 600, "Puzzle Maker"};
}

