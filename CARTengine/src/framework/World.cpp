
#include "World.h"
#include "Application.h"
#include "Core.h"
#include "Actor.h"
#include "HUD.h"
#include "Clock.h"

namespace cart {

	World::World(Application* owningApp)
		:m_owningApp{ owningApp },
		m_BeginPlay{false},
		m_Actors{},
		m_PendingActors{},
		m_cleanCycleStartTime(0),
		m_cleanCycleIter{ 5.f }
		
	{
		
	}

	void World::Init() {

		m_cleanCycleStartTime = Clock::Get().ElapsedTime();
	}


	void World::Update(float _deltaTime)
	{
#pragma region Update All Actors, Move destroyed actors to pending list
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{		
			// Move Actors to Pending Destroy list
			iter->get()->Update(_deltaTime);
			if (iter->get()->IsPendingDestroy() == true) {
				m_PendingActors.push_back(std::move(*iter));
				iter = m_Actors.erase(iter);
			}
			else {
				++iter;
			}
		}
#pragma endregion

#pragma region Garbage Collection TImer
		double curTime = Clock::Get().ElapsedTime();
		double elapsetime = curTime - m_cleanCycleStartTime;
		long garbagesize = GetSizeOfPendingActors();

		if (garbagesize >= (double)5000 || elapsetime >= m_cleanCycleIter) {
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
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			iter->get()->Draw(_deltaTime);
			++iter;
		}

		if (mHUD)
		{
			if (!mHUD->HasInit())
				mHUD->NativeInit();

			mHUD->Draw(_deltaTime);
		}
	}
	
	
	

	Vector2 World::GetAppWindowSize() const
	{		
		return m_owningApp->GetWindowSize();
	}

	void World::CleanCycle() {
		for (auto iter = m_PendingActors.begin(); iter != m_PendingActors.end();)
		{
			if (iter->get()->IsPendingDestroy())
			{
			//	LOG("WORLD CleanClcyle %s ", iter->get()->GetID().c_str());
				iter = m_PendingActors.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void World::Unload()
	{
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			iter->get()->Destroy();
			++iter;
		}
	}

	long World::GetSizeOfPendingActors() {

		long memsize = sizeof(Actor);
		long arrsize = sizeof(m_PendingActors);
		long countarry = m_PendingActors.size();
		long totalmem = memsize * countarry;
		return totalmem;
	
	}
	
	World::~World() {
		
	}
}