#pragma once
#include <functional>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include "Core.h"
#include "utils/JSutils.h"
#include "DataFile.h"
#include "network/network.h"
#include "Object.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __EMSCRIPTEN__
extern void ToggleMobileKeyboard(bool flag);
#endif // !__EMSCRIPTEN__

#ifdef __cplusplus
}
#endif

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
using json = nlohmann::json;

namespace cart
{
	class HUD;
	class World;
	class Application 
	{
	public:
		Application(int _winWidth, int _winHeight, const std::string& title);
		virtual void Invoke();
		virtual void Init();
		virtual void Start();
		virtual void Run();
		virtual void QuitApplication();
		virtual DataFile& GetModel() { return m_dataModel; };
		virtual World* GetCurrentWorld();
		virtual std::string& GetAssetsPath();
		virtual std::string& GetStaticAssetsPath();		
		virtual std::string GetResourceDisplayPath();
		virtual float GetIconSize();
		virtual Camera GetCamera()const { return m_camera; };
		virtual void SetHttpAssetPath(std::string path);
		static unique<network> net;

		static Application* app;

		void SetHTTPCallback(char* id, char* response, char* data);
		void LoadAssetCallback(char* uid, char* url, unsigned char* data, int size);
		void OnWindowResize(int w, int h);
		void MobileKeyboardHide();
		KeyboardStatus GetTouchKeyboardStatus();
		void ToggleMobileWebKeyboard(std::string& text,
			KeyboardType type,
			bool autocorrect,
			bool multiline,
			bool secure,
			bool alert,
			const std::string& placeholder,
			int characterLimit);
		void NotifyMobileInput(char* input, int isBackspace);
		void NotifyMobileKeyboardInterupt();
		void MobileKeyboardInterupt();
		void LogTrace(char* str);
		void RemoveMobileInputListener(std::string id);
		json& SetEnviornmentSettings(char* _setting);
		DataFile& GetGameConfig(){ return m_gameConfig; };
		json& GetGameConfigJSON() { return m_config_json; };
		Vector2 GetWindowSize() const;
		bool m_exit;
		~Application();
		virtual void Destroy();
		
		template<typename WorldType>
		weak<WorldType> LoadWorld();
		
		template<typename ClassName>
		void RegisterListernerToMobileInput(std::string id, weak<Object> obj, void(ClassName::* callback)(char*, int));

	protected:
		virtual void Update(float deltaTime);
		virtual void Draw(float deltaTime);
		virtual void LateUpdate(float deltaTime);

		int m_winWidth;
		int m_winHeight;
		std::string m_title;
		float m_targetFrameRate;
		shared<World> m_CurrentWorld;
		std::string m_assetsdir;
		std::string m_assetsdir_web;
		std::string m_static_assetsdir;
		DataFile m_dataModel; 
		DataFile m_gameConfig;
		json m_config_json;
		Camera m_camera;

	private:
		Dictionary<std::string, std::function<bool(char*, int)>> m_mobileInputListeners;

		//shared<World> m_PendingWorld;
	};
	

	template<typename WorldType>
	weak<WorldType> Application::LoadWorld()
	{
		shared <WorldType> newWorld{ new WorldType{this} };
		m_CurrentWorld = newWorld;
		//m_CurrentWorld->BeginPlayInternal();
		return newWorld;
	}

	template<typename ClassName>
	void Application::RegisterListernerToMobileInput(std::string id, weak<Object> obj, void(ClassName::* callback)(char*, int))
	{
		auto found = m_mobileInputListeners.find(id);

		if (found != m_mobileInputListeners.end())
		{
			m_mobileInputListeners.erase(found);
//			Logger::Get()->Warn(std::format("Application::RegisterListernerToMobileInput() Warning! Listener - {} - is already added. ", id));
//			return;
		}
		// listener not found in lists of callbacks hence add 
		std::function<bool(char*, int)> callbackFunc = [obj, callback](char* input, int isBackspace)->bool
		{
			if (!obj.expired())
			{
				(static_cast<ClassName*>(obj.lock().get())->*callback)(input, isBackspace);
				return true;
			}
			return false;
		};
		m_mobileInputListeners.insert({ id, callbackFunc });
	}
}