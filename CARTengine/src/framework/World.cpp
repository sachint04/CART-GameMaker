
#include "World.h"
#include "Application.h"
#include "Core.h"
#include "Actor.h"
#include "HUD.h"
#include "Clock.h"
#include "GameStage.h"
#include <stdexcept>
#include "AssetManager.h"
#include <limits> // Required for std::numeric_limits
#include "component/InputController.h"
namespace cart {

	World::World(Application* owningApp)
		:m_owningApp{ owningApp },
		m_BeginPlay{false},
		m_Actors{},
		m_PendingActors{},
		m_cleanCycleStartTime(0),
		m_cleanCycleIter{ 5.f },
		m_gameStages{},
		m_currentStage{m_gameStages.end()}		
	{
		
	}

	void World::Init() {
		m_cleanCycleStartTime = Clock::Get().ElapsedTime();
		InitGameStages();
		StartStage();
	}


#pragma region GAME STAGE MANAGEMENT
	void World::InitGameStages()
	{
		m_inputController = new InputController{ };
		
	}

    void World::AllGameStagesFinieshed()
    {
		LOG("All game stages fnished.");
    }

    void World::NextGameStage()
	{
		//m_currentStage = m_gameStages.erase(m_currentStage);
		++m_currentStage;
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->StartStage();

		}else{

			AllGameStagesFinieshed();
		}
	}

	void World::PreviousGameStage()
	{
		//m_currentStage = m_gameStages.erase(m_currentStage);
		--m_currentStage;
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->StartStage();			
		}
		else{

			LOG("NO GameStage found!");
		}	
	}

	void World::JumpToGameStage(const std::string& stageid) {
		
		int counter = 0;
		for (auto iter = m_gameStages.begin(); iter != m_gameStages.end(); ++iter)
		{
			if (iter->get()->GetID().compare(stageid) == 0)
			{
				m_currentStage = iter;
				break;
			}			
		}
		m_currentStage->get()->StartStage();
	}
	
	void World::AddStage(const shared<GameStage>& newStage)
	{
		m_gameStages.push_back(newStage);
		
	}

	void World::StartStage()
	{
		m_currentStage = m_gameStages.begin();
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->StartStage();			
		}
	}
#pragma endregion

#pragma region LOOP
	void World::Update(float _deltaTime)
	{
		Camera cam = m_owningApp->GetCamera();
		UpdateCamera(&cam, CAMERA_PERSPECTIVE);
		
		for (size_t i = 0; i < m_Actors.size(); i++)
		{
			if(m_Actors.at(i).get()->IsPendingDestroy() == false)
				m_Actors.at(i).get()->Update(_deltaTime);
		}
		//if (m_Actors.size() > 0) {
		//	List<shared<Actor>>::iterator iter = m_Actors.begin();
		//	
		//	LOG("(> WORLD <) Update() %d", &iter);
		//	for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
		//	{
		//		if (!iter->get()->IsPendingDestroy()) {
		//			iter->get()->Update(_deltaTime);
		//		}
		//	}
		//}

		/*
		*/
//		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
//		{
//			iter->get()->Update(_deltaTime);
		//		++iter;
			//if (iter->get()->IsPendingDestroy() == true) {
			//	m_PendingActors.push_back(std::move(*iter));
			//	iter = m_Actors.erase(iter);											
			//}
			//else 
			//{
			//	++iter;
			//}				
	//	}


		if(m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->Update(_deltaTime);
		}
		
#pragma region Garbage Collection TImer
		double curTime = Clock::Get().ElapsedTime();
		double elapsetime = curTime - m_cleanCycleStartTime;
		long garbagesize = GetSizeOfPendingActors();

		if (elapsetime >= m_cleanCycleIter) {
			CleanCycle();
			m_cleanCycleStartTime  = Clock::Get().ElapsedTime();
		}
#pragma endregion

#pragma region Update HUD

		if (mHUD)
		{
			if (!mHUD->HasInit())
				mHUD->NativeInit();

			mHUD->Update(_deltaTime);
		}
#pragma endregion

	}

	void World::Draw(float _deltaTime)
	{
	
	
			ClearBackground(RAYWHITE);
			for (auto iter = m_Actors.begin(); iter != m_Actors.end();  ++iter)
			{
				iter->get()->Draw(_deltaTime);
			}

			if (mHUD)
			{
				if (!mHUD->HasInit())
					mHUD->NativeInit();

				mHUD->Draw(_deltaTime);
			}
	
	}
#pragma endregion

#pragma region Helper

	Vector2 World::GetAppWindowSize() const
	{		
		return m_owningApp->GetWindowSize();
	}

	long World::GetSizeOfPendingActors() {

		long memsize = sizeof(Actor);
		long arrsize = sizeof(m_PendingActors);
		long countarry = m_PendingActors.size();
		long totalmem = memsize * countarry;
		return totalmem;

	}

	


#pragma endregion
	
#pragma region CleanUp

	void World::CleanCycle() {
	//	LOG("(- WORLD -) CleanCycle() ");
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			//if (iter->get()->IsPendingDestroy() && iter->use_count() == 1)
			//	LOG("Actor %s with use count%lu \n", iter->get()->GetID().c_str(), iter->use_count());
			if (iter->use_count() == 1)
			{			
		//		LOG("Removing last instanece of actor %s with use count%lu \n", iter->get()->GetID().c_str(), iter->use_count());
				iter->reset();
				iter = m_Actors.erase(iter);
				
			}
			else
			{
				++iter;
			}
		}
	//	LOG("Current Live Actors count %zu", m_Actors.size());
	//	LOG("(- WORLD -) CleanCycle() END!");
		AssetManager::Get().CleanCycle();
	
	}

	void World::Unload()
	{
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			iter->get()->Destroy();
			++iter;
		}
	}
	World::~World() {
		
	}
#pragma endregion
	
}