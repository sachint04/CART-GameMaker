#pragma once
#include <any>
#include <raylib.h>
#include "utils/rlights.h"
#include "Object.h"
#include "utils/StringUtils.h"
#include "DefaultMaterial.h"
#include "Logger.h"
namespace cart
{

	class Actor;
	class Actor3D;
	class Application;
	class HUD;
	class GameStage;
	class InputController;
	class UICanvas;
	class UIElement;
	class World : public Object
	{

	public:
		World(Application* owningApp);
		virtual void Init();
		virtual void Start();		
		virtual weak<HUD> GetHUD();		
		virtual void Update(float _deltaTime);
		virtual void Draw(float _deltaTime);
		virtual void LateUpdate(float _deltaTime);
		virtual void AddStage(const shared<GameStage>& newStage);
		virtual void AllGameStagesFinished();
		virtual InputController* GetInputController() { return m_inputController; };
		virtual const InputController* GetInputController() const { return m_inputController; }

		virtual Application* GetApplication();
		virtual const Application* GetApplication() const { return m_owningApp; }
		virtual ~World();
		
		virtual weak<Object> SortChildrenByZindex()override;

		static bool m_APP_SHOULD_WAIT;
		Vector2 GetAppWindowSize() const;
		long GetSizeOfPendingActors();
		void CleanCycle();
		void Unload();
		void InitStage();
		void StartStage();
		void NextGameStage();
		void PreviousGameStage();
		void JumpToGameStage(const std::string& stageid);
		void UnloadAllShaders();
		void UnloadAllLights();
		bool UnloadShaderGlobal(std::string name);
		bool UnloadLight(std::string name);
		bool UnloadMaterialGlobal(std::string name);
		void MoveToPendingDestroy();
		weak<Light> AddLight(const std::string& name, LightType type, Vector3 pos, Vector3 target, Color col, Shader lshader);
		weak<Shader> LoadOrGetShader(const std::string& name, const std::string& vsPath, const std::string& fsPath);
		weak<UIElement> GetUIElementBefore(const std::string& id);
		weak<Object> Find(const std::string& id);
		weak<Light> GetLight(std::string name);
		weak<Shader> GetShader(std::string name);

		void SetSessionData(const std::string& name, std::any data);
		std::any GetSessionData(const std::string& name);
		void DestroySessionData();

		template<int index, typename... Args>
		auto&& get_arg_by_index(Args&&... args);
	
		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor(Args... args);

		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor3D(Args... args);
		
		template<typename HUDType, typename... Args>
		weak<HUDType> SpawnHUD(Args... args);
		static shared<UICanvas> UI_CANVAS;

		Dictionary<std::string, std::any> m_sessionData;

		InputController* m_inputController;
		

		template<typename MaterialType, typename... Args>
		weak<MaterialType> GetOrCreateMaterial( const std::string& name, Args... args);
	private:

		bool m_BeginPlay;
		float m_cleanCycleIter;
		double m_cleanCycleStartTime;
		
		Application* m_owningApp;
		Map<std::string, shared<Actor>> m_Actors;
		Map<std::string, shared<Actor3D>> m_Actors3D;
		std::vector<std::string > m_ActorKeys;
		std::vector<std::string > m_Actor3DKeys;
		List<shared<Actor>> m_PendingActors;
		List<shared<GameStage>> m_gameStages;
		shared<HUD> m_HUD;
		List<shared<GameStage>>::iterator  m_currentStage;
		Dictionary<std::string, shared<Material>> m_materialLibrary;
		Dictionary<std::string, shared<Light>> m_lights;
		Dictionary<std::string, shared<Shader>> m_shaders;
		Map<std::string, shared<Actor>> m_pendingDestroyActors;
		Map<std::string, shared<Actor3D>> m_pendingDestroyActors3D;


	};

	template<int index, typename... Args>
	auto&& World::get_arg_by_index(Args&&... args) {
		// Expands the parameter pack into parameters of std::forward_as_tuple
		// which returns a tuple-like object preserving reference types.
		// std::get<index>() then extracts the desired element by its compile-time index.
		return std::get<index>(std::forward_as_tuple(std::forward<Args>(args)...));
	}


	template<typename ActorType, typename...Args>
	weak<ActorType> World::SpawnActor(Args...args)
	{
		auto&& first = get_arg_by_index<0>(std::forward<Args>(args)...);
		std::string copyid = first;
		int count = 0;
		auto find = m_Actors.find(copyid);
		while (find != m_Actors.end())
		{
			count++;
			copyid = first + "_copy_" + std::to_string(count);
			find = m_Actors.find(copyid);
			Logger::Get()->Warn(std::format(" World::SpawnActor() Warning!! Duplicate Object ID {}", first));
		}
		first = copyid;
		shared<ActorType> newActor{ new ActorType(this, args...) };		
		m_Actors.insert({ first ,newActor});
		m_ActorKeys.push_back(first);
		return newActor;
	}

	template<typename ActorType, typename...Args>
	weak<ActorType> World::SpawnActor3D(Args...args)
	{
		auto&& first = get_arg_by_index<0>(std::forward<Args>(args)...);
		std::string copyid = first;
		int count = 0;
		auto find = m_Actors3D.find(copyid);
		while (m_Actors3D.find(copyid) != m_Actors3D.end())
		{
			count++;
			copyid = first + "_copy_" + std::to_string(count);
			find = m_Actors3D.find(copyid);
			Logger::Get()->Warn(std::format(" World::SpawnActor3D() Warning!! Duplicate Object ID {}", first));
		}
		first = copyid;
		shared<ActorType> newActor{ new ActorType(this, args...) };
		m_Actors3D.insert({ first ,newActor });
		m_Actor3DKeys.push_back(first);
		return newActor;
	}

	template<typename HUDType, typename ...Args>
	inline weak<HUDType> World::SpawnHUD(Args ...args)
	{
		shared<HUDType> newHUD{ new HUDType(this,args...) };
		m_HUD = newHUD;
		return newHUD;
	}


	template<typename MaterialType, typename... Args>
	weak<MaterialType> World::GetOrCreateMaterial(const std::string& name, Args... args) {

		std::string key = StringUtils::ToLower(name);

		auto found = m_materialLibrary.find(key);
		if (found != m_materialLibrary.end()) {
			return  std::static_pointer_cast<MaterialType>(found->second);;
		}
		// Create new instance and store it

		shared<MaterialType> newMat = std::make_shared<MaterialType>(std::forward<Args>(args)...);

		m_materialLibrary[key] = newMat;
		return newMat;
	}
}
