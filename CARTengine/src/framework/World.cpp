
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
#include "Logger.h"
namespace cart {

	World::World(Application* owningApp)
		:m_owningApp{ owningApp },
		m_BeginPlay{false},
		m_Actors{},
		m_PendingActors{},
		m_cleanCycleStartTime(0),
		m_cleanCycleIter{ 5.f },
		m_gameStages{},
		m_currentStage{m_gameStages.end()},
		m_inputController{}
	{
		
	}

	void World::Init() {	
		m_cleanCycleStartTime = Clock::Get().ElapsedTime();
		m_inputController = new InputController{ };
		m_HUD.get()->Init();
		InitStage();
		Application::app->Start();
	}

	void World::Start()
	{		
		m_HUD.get()->Start();
		m_HUD.get()->SetVisible(true);
		StartStage();

	}

#pragma region GAME STAGE MANAGEMENT


    void World::AllGameStagesFinished()
    {
		Logger::Get()->Push("All game stages fnished.");
    }



    void World::NextGameStage()
	{
		++m_currentStage;
		if (m_currentStage != m_gameStages.end())
		{			
			m_currentStage->get()->Init();

		}else{

			AllGameStagesFinished();
		}
	}

	void World::PreviousGameStage()
	{
		//m_currentStage = m_gameStages.erase(m_currentStage);
		--m_currentStage;
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->Init();			
		}
		else{

			//Logger::Get()->Push("NO GameStage found!");
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
		m_currentStage->get()->Init();
	}
	
	void World::AddStage(const shared<GameStage>& newStage)
	{
		m_gameStages.push_back(newStage);
		
	}
	void World::InitStage()
	{
		m_currentStage = m_gameStages.begin();
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->Init();
		}
	}
	void World::StartStage()
	{
		m_currentStage = m_gameStages.begin();
		if (m_currentStage != m_gameStages.end())
		{
			m_currentStage->get()->Start();			
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
		m_HUD->Update(_deltaTime);

#pragma endregion

	}

	void World::Draw(float _deltaTime)
	{
			ClearBackground(RAYWHITE);
			for (auto iter = m_Actors.begin(); iter != m_Actors.end();  ++iter)
			{				
				iter->get()->Draw(_deltaTime);
			}

			m_HUD->Draw(_deltaTime);
	}

	void World::LateUpdate(float _deltaTime)
	{
		for (size_t i = 0; i < m_Actors.size(); i++)
		{
			if (m_Actors.at(i).get()->IsPendingDestroy() == false)
				m_Actors.at(i).get()->LateUpdate(_deltaTime);
		}

#pragma region Update HUD

		if (m_HUD)
		{
			m_HUD->LateUpdate(_deltaTime);
		}
#pragma endregion
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
	void World::SetSessionData(void* data) 
	{
		m_sessionData = data; 
	}

	weak<HUD> World::GetHUD()
	{
		return m_HUD;
	}
#pragma endregion
	
#pragma region CleanUp

	void World::CleanCycle() {
	//	Logger::Get()->Push("(- WORLD -) CleanCycle() ");
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			//if (iter->get()->IsPendingDestroy() && iter->use_count() == 1)
			//	Logger::Get()->Push("Actor %s with use count%lu \n", iter->get()->GetID().c_str(), iter->use_count());
			if (iter->use_count() == 1)
			{			
				std::string id = iter->get()->GetID();
		//		Logger::Get()->Push("Removing last instanece of actor %s with use count%lu \n", iter->get()->GetID().c_str(), iter->use_count());
				iter->reset();
				iter = m_Actors.erase(iter);
				
			}
			else
			{
				++iter;
			}
		}
	//	Logger::Get()->Push("Current Live Actors count {}", m_Actors.size());
	//	Logger::Get()->Push("(- WORLD -) CleanCycle() END!");
		AssetManager::Get().CleanCycle();
	
	}

	void World::Unload()
	{
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			iter->get()->Destroy();
			++iter;
		}
		for (auto stage : m_gameStages)
		{		
			stage.get()->Destroy();
			stage.reset();
		}
	}
	World::~World() {


		std::cout << "World Ended!!" << std::endl;
	}
#pragma endregion
	
}