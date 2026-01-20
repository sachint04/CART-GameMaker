
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
#include "UICanvas.h"
#include "UIElement.h"
namespace cart {

	bool World::m_APP_SHOULD_WAIT = true;// IMP * APP WILL NOT START/("Run()") UNLESS TRUE 
	shared<UICanvas> World::UI_CANVAS = nullptr;
	
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
		m_HUD.get()->Init();
		InitStage();
		Start();// START WORLD
	}

	void World::Start()
	{		
		m_HUD.get()->Start();
		m_HUD.get()->SetVisible(true);
		World::m_APP_SHOULD_WAIT = false;
	}

#pragma region GAME STAGE MANAGEMENT


    void World::AllGameStagesFinished()
    {
		Logger::Get()->Trace("All game stages fnished.");
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

			Logger::Get()->Error(" World::PreviousGameStage() | NO GameStage found!");
		}	
	}

	void World::JumpToGameStage(const std::string& stageid) {
		
		int counter = 0;
		for (auto iter = m_gameStages.begin(); iter != m_gameStages.end(); ++iter)
		{
			if (iter->get()->GetId().compare(stageid) == 0)
			{
				m_currentStage = iter;
				break;
			}			
		}
		m_currentStage->get()->Init();
	}

	weak<UIElement> World::GetUIElementBefore(const std::string& id)
	{
		int found = -1;
		weak<UIElement> uielem;
		for (size_t i = 0; i < m_Actors.size(); i++)
		{
			
			if (m_Actors.at(i).get()->GetId().compare(id) == 0) {
				found  = --i;
				break;
			}
			else {
				if (m_Actors.at(i).get()->type().compare("UIElement") == 0) {
					uielem = std::static_pointer_cast<UIElement>(m_Actors.at(i).get()->GetWeakRef().lock());
				}
			}
		}
		
		return uielem;
	}

	weak<Object> World::Find(const std::string& id)
	{
		for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
		{
			if (iter->get()->GetId().compare(id) == 0) {
				return iter->get()->GetWeakRef();
			}
		}
		return shared<Object>{ nullptr };
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

	//		m_HUD->Draw(_deltaTime);
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
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			//if (iter->get()->IsPendingDestroy() && iter->use_count() == 1)
			if (iter->use_count() == 1)
			{			
				std::string id = iter->get()->GetId();
				iter->reset();
				iter = m_Actors.erase(iter);
				
			}
			else
			{
				++iter;
			}
		}
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