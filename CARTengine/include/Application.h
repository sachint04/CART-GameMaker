#pragma once
#include <raylib.h>
#include <nlohmann/json.hpp>
#include "Core.h"
#include "DataFile.h"
#include "network/network.h"

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
		static unique<network> net;

		static Application* app;

		void SetHTTPCallback(char* id, char* response, char* data);
		void LoadAssetCallback(char* uid, char* url, unsigned char* data, int size);
		void OnWindowResize(int w, int h);
		DataFile& GetGameConfig(){ return m_gameConfig; };
		json& GetGameConfigJSON() { return m_config_json; };
		Vector2 GetWindowSize() const;
		template<typename WorldType>
		weak<WorldType> LoadWorld();
		bool m_exit;
		~Application();
		virtual void Destroy();
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
}