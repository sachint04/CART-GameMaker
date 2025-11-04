#include <string>
#include "Application.h"
#include "Core.h"
#include "World.h"
#include "AssetManager.h"
#include "Clock.h"
#include "Logger.h"
#include "HUD.h"
#include "Logger.h"


namespace cart
{
	unique<network> Application::net{ nullptr };
	Application* Application::app = { nullptr };

	Application::Application(int _winWidth, int _winHeight, const std::string& title)
		:m_winWidth{ _winWidth },
		m_winHeight{ _winHeight },
		m_title{ title },
		m_CurrentWorld{ nullptr },
		m_HUD{nullptr},
		m_targetFrameRate{ 60 },
		m_exit{ false },
		m_assetsdir{},
		m_assetsdir_web{},
		m_static_assetsdir{},
		m_dataModel{},
		m_gameConfig{},
		m_camera{},
		m_config_json{}
		
	{
		
	}

	void Application::Init() {
		m_dataModel = {};
		net = unique<network>{ new network };
		InitWindow(m_winWidth, m_winHeight, m_title.c_str());
		SetTargetFPS(m_targetFrameRate);		
	}

	void Application::Start() {
		Run();
	}

	/// <summary>
	/// This is the first function all to Object
	/// </summary>
	void Application::Invoke()
	{
		// No implimantations
	}

	void Application::Run()
	{
		Logger::Get()->Push("APPLICATION  Run() !!");
		Clock::Get().Reset();
		while (m_exit == false)
		{
			ClearBackground(RAYWHITE);
			Clock::Get().Tick();
			double deltaTime = Clock::Get().DeltaTime();
			Logger::Get()->Update(deltaTime);
			Update(deltaTime);
			BeginDrawing();
			Draw(deltaTime);
			Logger::Get()->Draw(deltaTime);
			EndDrawing();
			LateUpdate(deltaTime);



			if (WindowShouldClose())m_exit = true;
		}
		Clock::Get().Release();
		m_CurrentWorld->Unload();
		AssetManager::Get().CleanCycle();
		AssetManager::Get().Unload();
		AssetManager::Get().Release();
		 net = nullptr;
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

	weak<HUD> Application::GetHUD()
	{
		return m_HUD;
	}

	void Application::QuitApplication()
	{
		m_exit = true;
	}

	World* Application::GetCurrentWorld()
	{
		return m_CurrentWorld.get();
	}

	std::string& Application::GetAssetsPath()
	{
		return m_assetsdir;
	}

	std::string& Application::GetStaticAssetsPath()
	{
		return m_static_assetsdir;
	}

	std::string Application::GetResourceDisplayPath()
	{
		std::string path = "";
		return path;
	}

	float Application::GetIconSize() {
		return 0;
	}
	void Application::SetHUD(shared<HUD> hud)
	{
		m_HUD = hud;
	}

	void Application::Destroy() {
		m_CurrentWorld.get()->Unload();
		m_CurrentWorld.get()->Destroy();
		m_CurrentWorld.reset();
		m_HUD.get()->Destroy();
		m_HUD.reset();
	}
	Application::~Application()
	{
		std::cout << "Application Ended." << std::endl;
	}
	void Application::SetHTTPCallback(char* uid, char* response, char* data)
	{
		std::string id = { uid };
		std::string res = { response };
		std::string netdata = { data };
	//	Logger::Get()->Push(std::format("FROM Application id {} response {} data {} ", uid, response, data));
		net->HTTPCallback(id, res, netdata);
	}

	void Application::LoadAssetCallback(char* uid, char* filestr, unsigned char* data, int size)
	{
		std::string id = { uid };	
		std::string url = { filestr };	
		//	Logger::Get()->Push(std::format("FROM Application id {} response {} data {} ", uid, response, data));
		net->LoadAssetHTTPCallback(id, url, data, size);
	}
	
}