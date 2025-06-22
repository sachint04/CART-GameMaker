#pragma once
#include <raylib.h>
#include "Core.h"
#include "DataFile.h"
#include "network/network.h"
namespace cart
{
	class HUD;
	class World;
	class Application 
	{
	public:
		Application(int _winWidth, int _winHeight, const std::string& title);
		virtual void Init();
		virtual void BeginPlay();
		virtual void Run();

		Vector2 GetWindowSize() const ;


		template<typename WorldType>
		weak<WorldType> LoadWorld();
		bool m_exit;
		virtual void QuitApplication();
		virtual DataFile& GetModel() { return m_Model; };
		virtual World* GetCurrentWorld();
		void SetHTTPCallback(char* id, char* response, char* data);
		DataFile& GetGameConfig(){ return m_gameConfig; };


		virtual std::string& GetResourcePath();
		virtual std::string GetResourceDisplayPath();
		virtual float GetIconSize();
		virtual Camera GetCamera()const { return m_camera; };

		network* GetNet() { return net; };
	protected:
		
		virtual void Draw(float deltaTime);
		virtual void Update(float deltaTime);
		int m_winWidth;
		int m_winHeight;
		std::string m_title;
		shared<HUD> hud;
		float m_targetFrameRate;
	
		shared<World> m_CurrentWorld;
		std::string m_resourcedir;
		DataFile m_Model; 
		DataFile m_gameConfig;
		Camera m_camera;
		network* net;
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