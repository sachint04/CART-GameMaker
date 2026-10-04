#include <raylib.h> 
#include "World.h"
#include "Application.h"
#include "Core.h"
#include "Actor.h"
#include "Actor3D.h"
#include "HUD.h"
#include "Clock.h"
#include "tweenray/tween.h"
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
	
#pragma region Constructor Init & Start
	World::World(Application* owningApp)
		:m_owningApp{ owningApp },
		m_BeginPlay{false},
		m_Actors{},
		m_PendingActors{},
		m_cleanCycleStartTime(0),
		m_cleanCycleIter{ 5.f },
		m_gameStages{},
		m_currentStage{m_gameStages.end()},
		m_inputController{},
		m_pendingDestroyActors{},
		m_pendingDestroyActors3D{}
	{
		
	}
	void World::Init() {	
		m_Actors.clear();
		m_ActorKeys.clear();
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
#pragma endregion

#pragma region GAME STAGE MANAGEMENT
    void World::AllGameStagesFinished()
    {
		//Logger::Get()->Trace("All game stages fnished.");
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

			//Logger::Get()->Error(" World::PreviousGameStage() | NO GameStage found!");
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
	bool World::UnloadShaderGlobal(std::string name)
	{
		auto find = m_shaders.find(name);
		if (find != m_shaders.end())
		{
			if (find->second.use_count() > 1)
			{
				Logger::Get()->Warn(std::format("World::UnloadShader() : {} is still in use by {} other objects!",name , find->second.use_count() - 1));
				return false;
			}
			m_shaders.erase(find);
			return true;
		}
		return false;
	}
	bool World::UnloadLight(std::string name)
	{
		auto find = m_lights.find(name);
		if (find != m_lights.end())
		{
			if (find->second.use_count() > 1)
			{
				Logger::Get()->Warn(std::format(" World::UnloadLight(): {} is still in use by {} other objects!", name, find->second.use_count() - 1));
				return false;
			}
			m_lights.erase(find);
		}
		return true;
	}

	bool World::UnloadMaterialGlobal(std::string name)
	{
		std::string key = StringUtils::ToLower(name);

		auto found = m_materialLibrary.find(key);
		if (found != m_materialLibrary.end()) 
		{		
			found->second.reset();
			m_materialLibrary.erase(found);
			return true;
		}
		return false;
	}
	void World::MoveToPendingDestroy()
	{
		// Remove hangging keys from Actors 3D
		std::erase_if(m_Actor3DKeys, [this](const std::string& key) {
			if (key.empty()) return true; // Clean up empty keys immediately

			auto it = m_Actors3D.find(key);
			if (it != m_Actors3D.end() && it->second && it->second->IsPendingDestroy()) {
				// Move node to the pending destruction map
				auto node = m_Actors3D.extract(it);
				m_pendingDestroyActors3D.insert(std::move(node));
			//	Logger::Get()->Trace(std::format("World::MoveToPendingDestroy() {}", key));
				return true; // Erase key from m_Actor3DKeys
			}
			return false; // Keep key in vector
		});

		/*for (auto key : m_ActorKeys)
		{
			Logger::Get()->Trace(std::format("World::MoveToPendingDestroy() BEFORE {}", key));
		}*/
		// Remove hangging keys from Actors 2D
		std::erase_if(m_ActorKeys, [this](const std::string& key) {
			if (key.empty()) return true; // Clean up empty keys immediately

			auto it = m_Actors.find(key);
			if (it != m_Actors.end() && it->second && it->second->IsPendingDestroy()) {
				// Move node to the pending destruction map
				auto node = m_Actors.extract(it);
				m_pendingDestroyActors.insert(std::move(node));
			//	Logger::Get()->Trace(std::format("World::MoveToPendingDestroy() {}", key));
				return true; // Erase key from m_Actor3DKeys
			}
			return false; // Keep key in vector
		});
		/*for (auto key : m_ActorKeys)
		{
			Logger::Get()->Trace(std::format("World::MoveToPendingDestroy() AFTER {}", key));
		}*/
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
		
		UpdateCamera(&Application::CAMERA, Application::CAMERA_MODE);

		
		UI_CANVAS->Update(_deltaTime);
		#pragma region Update HUD
				m_HUD->Update(_deltaTime);
		#pragma endregion
		
		Tween::Update(_deltaTime);

		if (m_HUD->IsPopupActive())return;// pause game while popup is active
	
		for (size_t i = 0; i < m_Actor3DKeys.size(); i++)
		{
			auto key = m_Actor3DKeys[i];
			if (!key.empty()) 
			{
				auto it = m_Actors3D.find(key);
				if (it != m_Actors3D.end())
				{
					auto& actor = it->second;
					if (actor) 
					{
						auto parent = actor->GetParent().lock();
						if (!parent)
						{
							actor->Update3D(_deltaTime);
						}
					}
				}				
			}
		}


		for (size_t i = 0; i < m_ActorKeys.size(); i++)
		{		
			auto key = m_ActorKeys[i];
			if (!key.empty()) 
			{
				auto it = m_Actors.find(key);
				if (it != m_Actors.end())
				{
					auto& actor = it->second;

					if (actor) 
					{					
						auto parent = actor->GetParent().lock();
						if (!parent || !parent->IsUI())
						{
							actor->Update(_deltaTime);
						}					
					}					
				}
				
			}
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



	}
	void World::Draw(float _deltaTime)
	{
			ClearBackground(WHITE);
			
			BeginMode3D(Application::CAMERA);
			m_owningApp->ApplyCustomClipping(0.5f, 100.f);
			
	
			for (auto key : m_Actor3DKeys)
			{
				auto actor  = m_Actors3D[key];
				if (actor && !actor->IsPendingDestroy()) {
					auto parent = actor->GetParent().lock();

					// Only draw if there is no parent
					if (!parent)
					{
						if (actor->IsInstanced()) {
							actor->Update3D(_deltaTime); //Update and draw Batched models together	
						}
						actor->Draw(_deltaTime);
						actor->Draw3D(_deltaTime);
					}
				}
			}
			EndMode3D();

			for (size_t i = 0; i < m_ActorKeys.size(); i++)
			{
				auto actor = m_Actors[m_ActorKeys[i]];
				if (actor && !actor->IsPendingDestroy()) {
					auto parent = actor->GetParent().lock();
					
					// Only draw if there is no parent, or if the parent is NOT a UIElement
					if (!parent || !parent->IsUI())
					{
						actor->Draw(_deltaTime);
					}					
				}
			}

			m_HUD->Draw(_deltaTime);
	}
	void World::LateUpdate(float _deltaTime)
	{
		for (size_t i = 0; i < m_ActorKeys.size(); i++)
		{
			if (!m_Actors[m_ActorKeys[i]].get()->IsPendingDestroy())
				m_Actors[m_ActorKeys[i]].get()->LateUpdate(_deltaTime);
		}

#pragma region Update HUD

		/*if (m_HUD)
		{
			m_HUD->LateUpdate(_deltaTime);
		}*/
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
	void World::SetSessionData(const std::string& name, std::any data)
	{
		auto find = m_sessionData.find(name);
		if (find != m_sessionData.end()) {
			//Logger::Get()->Warn(std::format("World::SetSessionData() data found in session with name {} ", name));
		}
		m_sessionData[name] = std::move(data);
	}
	std::any World::GetSessionData(const std::string& name)
	{
		auto find = m_sessionData.find(name);
		if (find != m_sessionData.end()) {
			return find->second;
		}
		return std::any{}; 
	}
	weak<HUD> World::GetHUD()
	{
		return m_HUD;
	}
	weak<Object> World::SortChildrenByZindex()
	{
		std::sort(m_ActorKeys.begin(), m_ActorKeys.end(),
			[&](const std::string& a, const std::string& b) {
			weak<Actor> actorA = m_Actors.at(a);
			weak<Actor> actorB = m_Actors.at(b);
			bool aIsRoot = false, bIsRoot = false;
			auto alock = actorA.lock(), block = actorB.lock();
			if (alock) {
				aIsRoot = (alock->GetParent().lock() == nullptr);
			}
			if (block) {
				bIsRoot = (block->GetParent().lock() == nullptr);
			}

			// 1. Prioritize Roots: If one is a root and the other isn't, root comes first.
			if (aIsRoot != bIsRoot) {
				return aIsRoot; // Returns true if 'a' is the root, putting it first.
			}

			// 2. Sort Roots: If both are roots, sort them by Z-Index.
			if (aIsRoot && bIsRoot) {
				return alock->GetZindex() < block->GetZindex();
			}

			// 3. Children: If both are children, their order doesn't matter here 
			// because their respective parents will handle their specific sorting.
			return false;
		}
		);

		return Object::SortChildrenByZindex();
	}
	weak<UIElement> World::GetUIElementBefore(const std::string& id)
	{

		weak<UIElement> uielem;
		std::vector<std::string>::iterator keyIter = std::find(m_ActorKeys.begin(), m_ActorKeys.end(), id);
		if (keyIter != m_ActorKeys.end()) {
			while (keyIter != m_ActorKeys.begin())
			{
				keyIter--;
				Map<std::string, shared<Actor>>::iterator actorIter = m_Actors.find(*keyIter);

				if (actorIter == m_Actors.end())
				{
					continue;
				}

				auto elem = std::dynamic_pointer_cast<UIElement>(actorIter->second);
				if (elem)
				{
					uielem = elem;
					break;
				}
			}

		}

		return uielem;
	}
	weak<Light> World::AddLight(const std::string& name, LightType type, Vector3 pos, Vector3 target, Color col, Shader shader)
	{
		auto find = m_lights.find(name);
		if (find != m_lights.end()) {
			return find->second;
		}
		if (m_lights.size() >= 4) {
			// Log a warning here if you have a logger
			return std::weak_ptr<Light>(); // Returns an empty/expired weak_ptr
		}

		auto light = std::make_shared<Light>(CreateLight(type, pos, target, col, shader));
		auto res = m_lights.insert({ name,light });
		return res.first->second;
	}
	weak<Shader> World::LoadOrGetShader(const std::string& name, const std::string& vsPath, const std::string& fsPath)
	{
		auto find = m_shaders.find(name);
		if (find != m_shaders.end()) {
			return find->second;
		}

		const char* vPath = vsPath.empty() ? nullptr : vsPath.c_str();
		const char* fPath = fsPath.empty() ? nullptr : fsPath.c_str();
		Shader shader = LoadShader(vPath, fPath);
		if (shader.id == 0) {
			// TraceLog is Raylib's built-in logger
			Logger::Get()->Warn(std::format("SHADER: {} Failed to load. Using default shader.", name));
			UnloadShaderGlobal(name);//Raylib Function
			return std::weak_ptr<Shader>();
		}

		auto res = m_shaders.insert({ name, std::make_shared<Shader>(shader) });
		return res.first->second;
	}
	weak<Light> World::GetLight(std::string name)
	{
		auto it = m_lights.find(name);
		if (it != m_lights.end()) {
			return it->second;
		}
		return weak<Light>();
	}
	weak<Shader> World::GetShader(std::string name)
	{
		auto it = m_shaders.find(name);
		if (it != m_shaders.end()) {
			return it->second;
		}
		return weak<Shader>();
	}
	weak<Object> World::Find(const std::string& id)
	{
		auto found = m_Actors.find(id);
		if (found != m_Actors.end()) {
			return found->second.get()->GetWeakRef();
		}
		return shared<Object>{ nullptr };
	}

	Application* World::GetApplication()
	{
		return m_owningApp;
	}
#pragma endregion
	
#pragma region CleanUp
	void World::DestroySessionData()
	{
		m_sessionData.clear();
	}
	void World::CleanCycle() {
		
		for (auto keyIter = m_pendingDestroyActors.begin(); keyIter != m_pendingDestroyActors.end(); )
		{
			if (keyIter->second.use_count() > 1) {
				Logger::Get()->Error(std::format("World::CleanCycle() Actor {} with has dangling references", keyIter->second->GetId()));
			}
			keyIter->second.reset();
			keyIter = m_pendingDestroyActors.erase(keyIter);
		}

		for (auto keyIter = m_pendingDestroyActors3D.begin(); keyIter != m_pendingDestroyActors3D.end();)
		{
			if (keyIter->second.use_count() > 1) {
				Logger::Get()->Error(std::format("World::CleanCycle() Actor {} with has dangling references", keyIter->second->GetId()));
			}
			keyIter->second.reset();
			keyIter = m_pendingDestroyActors3D.erase(keyIter);			
		}
		m_pendingDestroyActors.clear();
		m_pendingDestroyActors3D.clear();
		for (auto iter = m_materialLibrary.begin(); iter != m_materialLibrary.end();) {

			if (iter->second->IsPendingDestroy()) {
				iter->second.reset();
				iter = m_materialLibrary.erase(iter);
			}
			else {
				iter++;
			}
		}

		
		m_HUD->CleanCycle();
		AssetManager::Get().CleanCycle();	
	}
	void World::Unload()
	{
		// Unload 2D objects
		auto actorsToDrain = std::move(m_Actors);
		m_Actors.clear();

		while (!actorsToDrain.empty()) {
			auto iter = actorsToDrain.begin();
			auto actor = iter->second;
			actorsToDrain.erase(iter);
			if (actor) {
				actor->Destroy();
			}
		}
		// Clear 3D objects
		auto actors3DToDrain = std::move(m_Actors3D);
		m_Actors3D.clear();
		while (!actors3DToDrain.empty()) {
			auto iter = actors3DToDrain.begin();
			auto actor = iter->second;
			actors3DToDrain.erase(iter);
			if (actor) {
				actor->Destroy();
			}
		}
		
		// Remove all Stages
		auto tmpStages = std::move(m_gameStages);
		m_gameStages.clear();
		while (!tmpStages.empty()) {
			auto iter = tmpStages.begin();
			auto stage = iter->get();
			
			if (stage) {
				stage->Destroy();
			}
			tmpStages.erase(iter);
		}
		/*for (auto stage : m_gameStages)
		{		
			if (stage) {
				stage->Destroy();
				stage.reset();
			}
		}*/

		UnloadAllLights();
		UnloadAllShaders();
		DestroySessionData();
	}
	void World::UnloadAllShaders()
	{
		for (auto iter = m_shaders.begin(); iter != m_shaders.end();)
		{
			UnloadShader(*iter->second);
			iter = m_shaders.erase(iter);
		}
	}
	void World::UnloadAllLights()
	{
		m_lights.clear();
	}

	World::~World() {
	
		std::cout << "World Ended!!" << std::endl;
	}

#pragma endregion
	
}