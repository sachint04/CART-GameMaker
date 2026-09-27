#include "Actor.h"
#include "stdexcept"

#include "Clock.h"
#include "World.h"
#include "AssetManager.h"
#include "Logger.h"
#include "component/IComponent.h"
#include "UICanvas.h"


namespace cart {
#pragma region Constructor & INIT


	Actor::Actor( World* _owingworld, const std::string& _id ) :Object{ _id },
		m_owningworld{_owingworld},
		m_scale(1.f), 
		m_location{}, 
		m_visible{ false }, 
		m_rotation(0),
		m_active{true},
		m_color{WHITE},
		m_width{1.f},
		m_height{1.f},
		m_location3{0},
		m_rotation3{0},
		m_zSize{1.f},
		m_preloadlist{},
		m_isReady{false},
		m_strloadMsg{"Loading... "},
		m_isLockedScale{false},
		m_areAssetsLoaded{false},
		m_areChildrenReady{false},
		m_rawWidth{},
		m_rawHeight{},
		m_tweening{false},
		m_zIndex{0},
		m_padding{ 0 }
	{
	}
	void Actor::Start()
	{		
		if(m_areChildrenReady && m_areAssetsLoaded && !m_isReady)
		SetReady(true);
	}
	void Actor::Init()
	{
		LoadAssets_async();
		
	}

#pragma endregion

#pragma region CleanUp

	void Actor::Destroy()
	{
		if (m_isPendingDestroy)return;
		m_customData.clear();
		for (auto iter = m_preloadlist.begin(); iter != m_preloadlist.end();)
		{
			if (iter->type == ASSET_IMAGE) {
				bool islocked = AssetManager::Get().IsAssetLocked(iter->virtualpath, iter->type);
				if (!islocked) {
					AssetManager::Get().UnloadTextureAsset(iter->virtualpath);
				}
			}
			else if (iter->type == ASSET_MODEL) {
				if (iter->texture_status == UNLOCKED) {
					// TBD
				}
			}
			iter = m_preloadlist.erase(iter);
		}
		m_preloadlist.clear();

		onReady.Destroy();
		Object::Destroy();
		// move all items to pending destroy
		m_owningworld->MoveToPendingDestroy();
	}

	Actor::~Actor()
	{
		//LOG("%s Actor deleted! ", m_id.c_str());
	}

#pragma endregion


#pragma region Healpers

	void Actor::SetReady(bool flag)
	{		
	//	Logger::Get()->Trace(std::format("[{}] is  ready !\n", GetId()));
		m_isReady = flag;
		onReady.Broadcast(GetId());
	}

	Vector2 Actor::GetWindowSize() const
	{
		return m_owningworld->GetAppWindowSize();
	}

	void Actor::SetCustomData(json data)
	{
		m_customData = data;
	}
		
	void Actor::SetLocation(Vector2 _location)
	{
		m_location = _location;
	}

	void Actor::SetLocation(Vector3 _location)
	{
		m_location3 = _location;
	}

	void Actor::Offset(Vector2 _location)
	{

		m_location.x += _location.x;
		m_location.y += _location.y;
	
	//	SetLocation(loc);
	}

	void Actor::SetScale(float _scale)
	{
		m_scale = _scale;
	}

	void Actor::SetScale3(Vector3 _scale)
	{
		// No implementation
	}

	void Actor::SetRotation(float _rotation)
	{
		m_rotation = _rotation;
	}

	void Actor::SetRotation3(Vector4 _rotation)
	{
		m_rotation3 = _rotation;
	}

	bool Actor::IsVisible() const
	{
		return m_visible;
	}

	bool Actor::IsActive() const
	{
		return m_active;
	}

	void Actor::SetActive(bool _flag)
	{
		m_active = _flag;
	}

	void Actor::SetColor(Color _color)
	{
		m_color = _color;
	}

	float Actor::GetRotation()
	{
		return m_rotation;
	}

	Color Actor::GetColor()
	{
		return m_color;
	}

	Vector2 Actor::GetLocation()
	{
		return m_location;
	}
	
	Vector3 Actor::GetLocation3()
	{
		return m_location3;
	}

	Vector4 Actor::GetRotation3()
	{
		return m_rotation3;
	}

	float Actor::GetScale()
	{
		return m_scale;
	}

	Vector2 Actor::GetSize()
	{
		return { m_width, m_height };
	}

	Vector3 Actor::GetSize3()
	{
		return { m_width, m_height, m_zSize };
	}

	Vector2 Actor::GetRawSize()
	{
		return { m_rawWidth, m_rawHeight };
	}
	
	void Actor::Update(float _deltaTime)
	{
		
		// Call this EVERY FRAME in your main loop (Native only)
#ifdef _WIN32
		std::vector<DelayedTask> tasksToRun;
		auto now = Clock::Get().SteadyTime();
		try {
			for (auto it = m_tasks.begin(); it != m_tasks.end(); ) {
				if (now >= it->targetTime) {
					if (it == m_tasks.end()) {
						throw std::runtime_error("Iterator corruption detected: Container was modified inside func()!");
					}
					tasksToRun.push_back(std::move(*it));
					//auto callback = it->func;
					//				it->func(); // Runs on Main Thread
					it = m_tasks.erase(it);

				
				}
				else {
					++it;
				}
			}
		}catch (const std::exception& e) {
			std::cerr << "Application Error: " << e.what() << std::endl;
			// Handle or safely log the error state here
		}
		for (const auto& task : tasksToRun) {
			if (task.func) {
				task.func();
			}
		}
#endif
	}

	void Actor::Draw(float _deltaTime)
	{
	}

	void Actor::LateUpdate(float _deltaTime)
	{
	}

	void Actor::SetSize(Vector2 _size)
	{
		m_width = _size.x;
		m_height = _size.y;
	}

	void Actor::UpdateRawSize(Vector2 _size)
	{
		m_rawWidth = _size.x;
		m_rawHeight = _size.y;
	}

	void Actor::SetSize(Vector3 _size)
	{
		m_width = _size.x;
		m_height = _size.y;
		m_zSize = _size.z;
	}

	/// <summary>
	/// Load Texture before rendering page
	/// </summary>
	void Actor::LoadAssets_async()
	{
		if (m_preloadlist.size() > 0) {			
			AssetManager::Get().LoadAssetList(GetId(), m_preloadlist, m_strloadMsg, GetWeakRef(), &Actor::AssetsLoadCompleted);
		}
		else {
			AssetsLoadCompleted();
		}		
	}

	void Actor::AssetsLoadCompleted()
	{
		Logger::Get()->Trace(std::format("{} Asset Load complete!", GetId()));
		m_areAssetsLoaded = true;
		Start();
	}

	

	std::string Actor::type()
	{
		return std::string{ "Actor" };
	}

	void Actor::SetTweening(bool flag)
	{
		m_tweening = flag;
	}

	weak<Object> Actor::GetParent()
	{
		return m_parentObj.lock();	
		
	}

	weak<Object> Actor::SortChildrenByZindex()
	{
		return Object::SortChildrenByZindex();
	}

	bool Actor::AddChild(weak<Actor> elem)
	{
		return false;
	}

	weak<Object> Actor::Child(const std::string& id)
	{
		return weak<Object>();
	}


	void Actor::SetZindex(int index, bool sort)
	{
		m_zIndex = index;
		
	}

	void Actor::SetMaterial(weak<Material> mat)
	{
		m_material = mat;
	}

	weak<Material> Actor::GetMaterial()
	{
		return m_material.lock();
	}

	void Actor::SetVisible(bool _flag)
	{
		m_visible = _flag;
	}

	void Actor::PostDelayed(std::function<void()> func, std::chrono::milliseconds delay) {
#ifdef __EMSCRIPTEN__
		// Web already handles the main-thread loop for us
		auto* f = new std::function<void()>(func);
		emscripten_async_call([](void* arg) {
			auto* task = static_cast<std::function<void()>*>(arg);
			(*task)();
			delete task;
		}, f, static_cast<int>(delay.count()));
#else
		// Native needs to be tracked manually
		m_tasks.push_back({ func, std::chrono::steady_clock::now() + delay });
#endif
	}

#pragma endregion


}
