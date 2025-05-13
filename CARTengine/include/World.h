#pragma once

#include <raylib.h>
#include "Core.h"
#include "Object.h"



namespace cart
{

	class Actor;
	class Application;
	class HUD;
	class GameStage;
	class World : public Object
	{

	public:
		World(Application* owningApp);
		virtual void Init();
		virtual void Update(float _deltaTime);
		virtual void Draw(float _deltaTime);
		virtual void AddStage(const shared<GameStage>& newStage);
		virtual ~World();

		//void BeginPlayInternal();

		Vector2 GetAppWindowSize() const;
		Application* GetApplication() { return m_owningApp; }
		const Application* GetApplication() const { return m_owningApp; }
		long GetSizeOfPendingActors();
		void CleanCycle();
		void Unload();
		void StartStage();
		void NextGameStage();
		void PreviousGameStage();

		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor(Args... args);
		
		template<typename HUDType, typename... Args>
		weak<HUDType> SpawnHUD(Args... args);
		
		private:
		
		virtual void InitGameStages();
		
		bool m_BeginPlay;
		float m_cleanCycleIter;
		double m_cleanCycleStartTime;

		Application* m_owningApp;

		List<shared<Actor>> m_Actors;
		List<shared<Actor>> m_PendingActors;
		List<shared<GameStage>> m_gameStages;
		shared<HUD> mHUD;
		List<shared<GameStage>>::iterator  m_currentStage;
		
		virtual void AllGameStagesFinieshed();
	};


	template<typename ActorType, typename...Args>
	weak<ActorType> World::SpawnActor(Args...args)
	{
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
