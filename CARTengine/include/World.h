#pragma once

#include <raylib.h>
#include "Core.h"
#include "Object.h"



namespace cart
{

	class Actor;
	class Application;
	class HUD;

	class World : public Object
	{

	public:
		World(Application* owningApp);
		virtual void Init();
		//void BeginPlayInternal();
		virtual ~World();

		Vector2 GetAppWindowSize() const;
		void Update(float _deltaTime);
		void Draw(float _deltaTime);
		Application* GetApplication() { return m_owningApp; }
		const Application* GetApplication() const { return m_owningApp; }
		long GetSizeOfPendingActors();
		void CleanCycle();
		void Unload();
		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor(Args... args);

		template<typename HUDType, typename... Args>
		weak<HUDType> SpawnHUD(Args... args);
	
	private:

		Application* m_owningApp;
		bool m_BeginPlay;
		float m_cleanCycleIter;
		double m_cleanCycleStartTime;
		List<shared<Actor>> m_Actors;

		List<shared<Actor>> m_PendingActors;
		shared<HUD> mHUD;
	};


	template<typename ActorType, typename...Args>
	weak<ActorType> World::SpawnActor(Args...args)
	{
		//std::cout << "args " << args... << std::endl;
		shared<ActorType> newActor{ new ActorType(this, args...) };
		m_Actors.push_back(newActor);
		return newActor;
	}

	template<typename HUDType, typename ...Args>
	inline weak<HUDType> World::SpawnHUD(Args ...args)
	{
		shared<HUDType> newHUD{ new HUDType(this,args...) };
		mHUD = newHUD;
		return newHUD;
	}

}
