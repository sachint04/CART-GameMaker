#include <raylib.h>
#include <rlgl.h> 
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
	#include <emscripten/emscripten.h>
	#include "utils/MobileKeyboard.h"
#endif // __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__	
	#include <GLES3/gl3.h>
#elif _WIN32

#else
	#include <GL/gl.h>   
#endif
namespace cart
{
	Camera Application::CAMERA = {0};
	int Application::CAMERA_MODE = CAMERA_PERSPECTIVE;
	unique<network> Application::net{ nullptr };
	Application* Application::app = { nullptr };
	int Application::GPT_TIER;

	double Application::CAMERA_NEAR_PLANE = 1.0f;
	double Application::CAMERA_FAR_PLANE = 50.0f;
	bool Application::BACK_FACE_CULLING = true;

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
		if(BACK_FACE_CULLING)rlEnableBackfaceCulling();
#ifdef __EMSCRIPTEN__
		int depthBits = 0;
		glGetIntegerv(GL_DEPTH_BITS, &depthBits);
		TraceLog(LOG_INFO, "Hardware Depth Buffer Bits: %i", depthBits);
#endif // __EMSCRIPTEN__
	}

	void Application::Start() {				
		Run();
	}

	void Application::MainLoopWrapper(void* arg) {
		static_cast<Application*>(arg)->UpdateDrawFrame();
	}

	void Application::UpdateDrawFrame()
	{
		Clock::Get().Tick();
		float deltaTime = GetFrameTime();
		Update(deltaTime);
		BeginDrawing();
			rlSetClipPlanes(CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);
			ClearBackground(RAYWHITE);
			Draw(deltaTime);
			Logger::Get()->Draw(deltaTime);
		EndDrawing();
		LateUpdate(deltaTime);	
		Logger::Get()->Update(deltaTime);
		if (WindowShouldClose()) {
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif // __EMSCRIPTEN__
			m_exit = true;
		}
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
#ifdef __EMSCRIPTEN__
			emscripten_set_main_loop_arg(Application::MainLoopWrapper, this, 0, 1);
#else
		SetTargetFPS(m_targetFrameRate);
		while (m_exit == false)
		{
			//Clock::Get().Tick();
			//double deltaTime = Clock::Get().DeltaTime();
			//Update(deltaTime);
			//Logger::Get()->Update(deltaTime);

			//BeginDrawing();
			//	ClearBackground(RAYWHITE);
			//	Draw(deltaTime);
			//	Logger::Get()->Draw(deltaTime);
			//EndDrawing();
			//	LateUpdate(deltaTime);
			UpdateDrawFrame();
		}
#endif // __EMSCRIPTEN__
		Clock::Get().Release();
		m_CurrentWorld->Unload();
		AssetManager::Get().CleanCycle();
		AssetManager::Get().Unload();
		AssetManager::Get().Release();
		 net = nullptr;
		Destroy();
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

	std::string Application::GetAssetsPath()
	{
		return m_assetsdir;
	}

	std::string Application::GetStaticAssetsPath()
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
	float Application::GetIconSize2() {
		return 0;
	}

	void Application::SetHttpAssetPath(std::string path)
	{
		m_assetsdir_web = path;
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
		Logger::Get()->Trace(std::format("Application::NotifyMobileInput() Success!! input {} | backspace {}", input, isBackspace));
		for (auto iter = m_mobileInputListeners.begin(); iter != m_mobileInputListeners.end(); ++iter)
		{
			if ((iter->second)(input, isBackspace))
			{
			}
			else {
				//Logger::Get()->Trace(std::format("Application::NotifyMobileInput() Failed :( input {} | backspace {}", input, isBackspace));
			}
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

	void Application::ApplyCustomClipping(double nearPlan, double farPlan)
	{
		float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
		// Use the nearPlan to calculate the frustum dimensions
		double top = nearPlan * tan(CAMERA.fovy * 0.5 * DEG2RAD);
		double right = top * aspect;

		rlMatrixMode(RL_PROJECTION);
		rlLoadIdentity();
		rlFrustum(-right, right, -top, top, nearPlan, farPlan);
		rlMatrixMode(RL_MODELVIEW);
	}

	json& Application::SetEnviornmentSettings(char* _setting)
	{	
		Logger::Get()->Trace(std::format("Application::SetEnviornmentSettings  {}", std::string{ _setting }));
		return CARTjson::readEnvSettings(std::string{ _setting });
	}
	
}