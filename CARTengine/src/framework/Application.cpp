#include "Application.h"
#include "Core.h"
#include "World.h"
#include "AssetManager.h"
#include "Clock.h"
#include <string>

namespace cart
{
	Application::Application(int _winWidth, int _winHeight, const std::string& title)
		:m_winWidth{ _winWidth },
		m_winHeight{ _winHeight },
		m_title{ title },
		m_CurrentWorld{ nullptr },
		m_targetFrameRate{ 60 },
		m_exit{ false },
		hud{},
		m_resourcedir{},
		m_Model{},
		m_gameConfig{},
		m_camera{},
		m_config_json{}
		
	{
		net = new network{};
	}

	void Application::Init() {
		m_Model = {};
		InitWindow(m_winWidth, m_winHeight, m_title.c_str());
		SetTargetFPS(m_targetFrameRate);
	}

	void Application::BeginPlay()
	{
	}

	void Application::Run()
	{
		Clock::Get().Reset();
		while (m_exit == false)
		{
			ClearBackground(RAYWHITE);
			Clock::Get().Tick();
			double deltaTime = Clock::Get().DeltaTime();
			Update(deltaTime);
			BeginDrawing();
			Draw(deltaTime);
			EndDrawing();
			LateUpdate(deltaTime);
			if (WindowShouldClose())m_exit = true;
		}
		Clock::Get().Release();
		m_CurrentWorld->Unload();
		AssetManager::Get().CleanCycle();
		AssetManager::Get().Unload();
		AssetManager::Get().Release();
		delete net;
		//int leak = _CrtDumpMemoryLeaks();
	
		CloseWindow();
	}


	Vector2 Application::GetWindowSize() const
	{
		return { (float)GetScreenWidth(), (float)GetScreenHeight() };
	}

	void Application::Update(float deltaTime) {
		m_CurrentWorld->Update(deltaTime);
	}

	void Application::Draw(float deltaTime) {
		m_CurrentWorld->Draw(deltaTime);
	}

	void Application::LateUpdate(float deltaTime) {
		m_CurrentWorld->LateUpdate(deltaTime);
	}

	void Application::QuitApplication()
	{
		m_exit = true;
	}

	World* Application::GetCurrentWorld()
	{
		return m_CurrentWorld.get();
	}

	std::string& Application::GetResourcePath()
	{
		return m_resourcedir;
	}

	

	std::string Application::GetResourceDisplayPath()
	{
		std::string path = "";
		return path;
	}

	float Application::GetIconSize() {
		return NULL;
	}
	void Application::SetHTTPCallback(char* uid, char* response, char* data)
	{
		std::string id = { uid };
		std::string res = { response };
		std::string netdata = { data };
		net->HTTPCallback(id, res, netdata);
		LOG("FROM Application id %s response %s data %s ", uid, response, data);
	}
	
}