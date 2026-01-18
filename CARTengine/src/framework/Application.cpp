#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include "Application.h"
#include "Core.h"
#include "World.h"
#include "component/InputController.h"
#include "AssetManager.h"
#include "Clock.h"
#include "Logger.h"
#include "HUD.h"
#include "Logger.h"
#include "CARTjson.h"


#ifdef __EMSCRIPTEN__
	#include "utils/MobileKeyboard.h"
#endif // __EMSCRIPTEN__

namespace cart
{
	unique<network> Application::net{ nullptr };
	Application* Application::app = { nullptr };

	Application::Application(int _winWidth, int _winHeight, const std::string& title)
		:m_winWidth{ _winWidth },
		m_winHeight{ _winHeight },
		m_title{ title },
		m_CurrentWorld{ nullptr },
		m_targetFrameRate{ 60 },
		m_exit{ false },
		m_assetsdir{},
		m_assetsdir_web{},
		m_static_assetsdir{},
		m_dataModel{},
		m_gameConfig{},
		m_camera{},
		m_config_json{},
		m_mobileInputListeners{}
	{
		
	}

	void Application::Init() {
		m_dataModel = {};	
		net = unique<network>{ new network };
	//	SetConfigFlags(FLAG_MSAA_4X_HINT|FLAG_VSYNC_HINT);
		//Logger::Get()->Trace(std::format("Application Init size {} | {} ", SCREEN_WIDTH, SCREEN_HEIGHT));
		InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, m_title.c_str());
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
		Logger::Get()->Trace("APPLICATION  Run() !!");
		Clock::Get().Reset();
		while (m_exit == false)
		{
			ClearBackground(RAYWHITE);
			Clock::Get().Tick();
			double deltaTime = Clock::Get().DeltaTime();
			Update(deltaTime);
			Logger::Get()->Update(deltaTime);

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

	void Application::Destroy() {
		m_CurrentWorld.get()->Unload();
		m_CurrentWorld.get()->Destroy();
		m_CurrentWorld.reset();
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
		net->HTTPCallback(id, res, netdata);
	}

	void Application::LoadAssetCallback(char* uid, char* filestr, unsigned char* data, int size)
	{
		std::string id = { uid };	
		std::string url = { filestr };	
		net->LoadAssetHTTPCallback(id, url, data, size);
	}
	
	void Application::OnWindowResize(int w, int h) {
		SetWindowSize(w, h);
	}
	void Application::MobileKeyboardHide() {
#ifdef __EMSCRIPTEN__
		MobileKeyboard::Hide(true);
#endif // __EMSCRIPTEN__
	}

	KeyboardStatus Application::GetTouchKeyboardStatus()
	{
#ifdef __EMSCRIPTEN__
		return MobileKeyboard::GetStatus();
#endif // __EMSCRIPTEN__
		return KeyboardStatus::Done;
	}

	void Application::ToggleMobileWebKeyboard(std::string& text,
		KeyboardType type,
		bool autocorrect,
		bool multiline,
		bool secure,
		bool alert,
		const std::string& placeholder,
		int characterLimit)
	{		
#ifdef __EMSCRIPTEN__
		MobileKeyboard::TouchStart(text, type, autocorrect, multiline, secure, alert, placeholder, characterLimit);
#endif // __EMSCRIPTEN__

	}

	void Application::NotifyMobileInput(char* input, int isBackspace)
	{
		//Logger::Get()->Trace(std::format("Application::NotifyMobileInput() input {} | backspace", input, isBackspace));
		for (auto iter = m_mobileInputListeners.begin(); iter != m_mobileInputListeners.end(); ++iter)
		{
			if ((iter->second)(input, isBackspace))
			{

			};
		}
	}
	/// <summary>
	/// Clear mobile keyboard listeners and unset focus from all elements;
	/// </summary>
	void Application::NotifyMobileKeyboardInterupt()
	{
		for (auto iter = m_mobileInputListeners.begin(); iter != m_mobileInputListeners.end();)
		{
			iter = m_mobileInputListeners.erase(iter);
		}
		m_CurrentWorld.get()->GetInputController()->SetFocus("");
	}

	void Application::MobileKeyboardInterupt()
	{
#ifdef __EMSCRIPTEN__
		MobileKeyboard::InteruptTouch();
#endif // __EMSCRIPTEN__

	}

	void Application::LogTrace(char* str)
	{
		Logger::Get()->Trace(std::string{ str });
	}

	void Application::RemoveMobileInputListener(std::string id)
	{
		auto found = m_mobileInputListeners.find(id);
		if (found != m_mobileInputListeners.end()) {
			m_mobileInputListeners.erase(found);
		}
	}

	json& Application::SetEnviornmentSettings(char* _setting)
	{	
		Logger::Get()->Trace(std::format("Application::SetEnviornmentSettings  {}", std::string{ _setting }));
		return CARTjson::readEnvSettings(std::string{ _setting });
	}
	
}