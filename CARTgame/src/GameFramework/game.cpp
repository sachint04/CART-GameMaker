#include <raylib.h>
#include <stdexcept>
#include <iostream>
#include "game.h"
#include "CARTjson.h"
#include "AssetManager.h"
#include "HUD.h"
#include "World.h"
#include "Levelone.h"
#include "gameplayHUD.h"
#include "cartconfig.h"
#include "Logger.h"
#include "UICanvas.h"
#include "component/InputController.h"

//using namespace std;
using namespace cart;

//#define __LOGGER__ // Custom Logger view

extern int DEFAULT_CANVAS_WIDTH;
extern int DEFAULT_CANVAS_HEIGHT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

float APP_SCALE;
int FONT_SIZE;
std::string FONT_NAME = "fonts/framd.ttf";
namespace cart {

    // Constructor
    Game::Game(int _winWidth, int _winHeight, const std::string& title)
        : Application{ _winWidth ,_winHeight, title },
        logger{nullptr}
        
    {
        app = this;
       
    }

#pragma region App Initialization

    void Game::Init() {

        m_assetsdir = GetResourceDir();
#ifdef __EMSCRIPTEN__
        m_static_assetsdir = "assets/";

        std::string href = net->GetURL();
        int find = href.find_last_of("/");
        std::string host = href;
        if (find != std::string::npos)
        {
            host = href.substr(0, find);
        }
        m_assetsdir_web = host + "/" + m_assetsdir;
#endif
#ifdef  _WIN32
        m_static_assetsdir = m_assetsdir;
#endif //  _WIN32
      
#pragma region LOGGER
        logger = Logger::Get(); // LOGGER
        Rectangle loggerRect = { 25.0f, 500.0f, 600.f, 100.f };
        int maxlinecount = 50;
        logger->SetRect(loggerRect);
        logger->SetMaxLogCount(maxlinecount);
#ifndef __LOGGER__
        logger->Hide();
#endif // __LOGGER__
#pragma endregion

        m_camera = { { 0.0f, 0.01f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.f, CAMERA_PERSPECTIVE };// CREATE CAMERA
        Application::Init(); // CREATE APPLIATION WINDOW

        // CREATE NEW WORLD
        weak<Levelone> newWorld = LoadWorld<Levelone>();
        newWorld.lock()->m_inputController = new InputController{ };

        weak<UICanvas> ui_canvas = newWorld.lock().get()->SpawnActor<UICanvas>(std::string{ "ui_canvas" });
        World::UI_CANVAS = ui_canvas.lock();
        ui_canvas.lock().get()->Init();
        ui_canvas.lock().get()->SafeRect({ 0.f,0.f, (float)DEFAULT_CANVAS_WIDTH, (float)DEFAULT_CANVAS_HEIGHT });
        ui_canvas.lock().get()->SetVisible(true);

        weak<GameplayHUD> hud = newWorld.lock().get()->SpawnHUD<GameplayHUD>(std::string{ "HUD" }); // HUD

        m_CurrentWorld->Init(); // INIT WORLD

      

        // SET APP SCALE
        APP_SCALE = (m_winWidth < m_winHeight) ? (float)m_winWidth / (float)m_winHeight : (float)m_winHeight / (float)m_winWidth;
        FONT_SIZE = 24 * APP_SCALE;

        while (World::m_APP_SHOULD_WAIT) {
            std::cout << "Application is waiting for World to be Ready!!" << std::endl;
        }

        SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        World::UI_CANVAS.get()->UpdateLayout();

#pragma endregion
        Application::Start();
        Logger::Get()->Trace("Application Started!!!\0");

    }

    std::string& Game::GetAssetsPath()
    {
#ifdef __EMSCRIPTEN__
        return  m_assetsdir_web;
#endif
        return m_assetsdir;
        // TODO: insert return statement here
    }

    std::string Game::GetResourceDisplayPath()
    {
        int w = GetScreenWidth();
        auto& config = CARTjson::GetAppData();
        std::string result = "";
        if (w <= 320)
        {
            result = config["cart"]["display"]["xsmall"]["path"];
        }
        else if (w > 320 && w <= 420)
        {
            result = config["cart"]["display"]["small"]["path"];
        }
        else if (w > 420 && w <= 768)
        {
            result = config["cart"]["display"]["medium"]["path"];
        }
        else if (w > 768 && w <= 1024)
        {
            result = config["cart"]["display"]["large"]["path"];
        }
        else if (w > 1024 && w <= 2960)
        {
            result = config["cart"]["display"]["xlarge"]["path"];
        }
        result = GetAssetsPath() + result;


        return result;
    }

    float Game::GetIconSize()
    {
        int w = GetScreenWidth();
        auto& config = CARTjson::GetAppData();
        if (w <= 320)
        {
            return (float)config["cart"]["display"]["xsmall"]["iconsize"];
        }
        else if (w > 320 && w <= 420)
        {
            return (float)config["cart"]["display"]["small"]["iconsize"];
        }
        else if (w > 420 && w <= 768)
        {
            return (float)config["cart"]["display"]["medium"]["iconsize"];
        }
        else if (w > 768 && w <= 1024)
        {
            return (float)config["cart"]["display"]["large"]["iconsize"];
        }
        else if (w > 1024 && w <= 2960)
        {
            return (float)config["cart"]["display"]["xlarge"]["iconsize"];
        }
        return 0;
    }



#pragma endregion






#pragma region CLEAN UP

    Game::~Game() {
        Application::Destroy();
        delete logger;

    }
#pragma endregion

}
cart::Application* app;

cart::Application* GetApplication()
{
    if (!app) {
        app = new cart::Game{ SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle Maker" };
    }
    return app;
}

