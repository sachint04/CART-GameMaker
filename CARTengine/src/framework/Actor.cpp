#include "Actor.h"
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
		m_isLockedScale{false}
	{
	}
	void Actor::Start()
	{
		SetReady(true);
	}
	void Actor::Init()
	{
		LoadAssets();
		
	}

#pragma endregion

#pragma region CleanUp


	Actor::~Actor()
	{
		//LOG("%s Actor deleted! ", m_id.c_str());
	}

#pragma endregion


#pragma region Healpers

	void Actor::SetReady(bool flag)
	{
		m_isReady = flag;
		onReady.Broadcast(GetId());
	}
	void Actor::AddComponent(const std::string& id, weak<IComponent> component)
	{
		m_componentlist.insert({id, component });
	}

	weak<IComponent> Actor::GetComponentById(const std::string& id)
	{
		auto find = m_componentlist.find(id);
		if (find != m_componentlist.end()) {
			return find->second;
		}
		return  shared<IComponent>{nullptr};
	}

	bool Actor::HasComponent(COMPONENT_TYPE type)
	{
		for (auto iter = m_componentlist.begin(); iter != m_componentlist.end(); iter++)
		{
			if (iter->second.lock()->GetType() == type) {
				return true;
			}
		}
		return false;
	}

	Vector2 Actor::GetWindowSize() const
	{
		return m_owningworld->GetAppWindowSize();
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
		Vector2 loc = { m_location.x + _location.x, m_location.y + _location.y };
		SetLocation(loc);
	}

	void Actor::SetScale(float _scale)
	{
		m_scale = _scale;
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
	
	void Actor::Update(float _deltaTime)
	{
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

	void Actor::SetSize(Vector3 _size)
	{
		m_width = _size.x;
		m_height = _size.y;
		m_zSize = _size.z;
	}

	/// <summary>
	/// Load Texture before rendering page
	/// </summary>
	void Actor::LoadAssets()
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
		Start();
	}

	void Actor::Destroy()
	{
		if (m_isPendingDestroy)return;
		for (auto iter = m_componentlist.begin(); iter != m_componentlist.end();)
		{
			 World::UI_CANVAS.get()->RemoveComponent(iter->first);
			iter->second.lock()->Destroy();
			iter = m_componentlist.erase(iter);
		}
		m_preloadlist.clear();
		Object::Destroy();
	}

	void Actor::SetVisible(bool _flag)
	{
		m_visible = _flag;
	}

#pragma endregion


}
