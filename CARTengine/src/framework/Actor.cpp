#include "Actor.h"
namespace cart {
	Actor::Actor( World* _owingworld, const std::string& _id ) :Object{ _id },
		m_owningworld{_owingworld},
		m_scale(1.f), 
		m_location{}, 
		m_visible{ false }, 
		m_rotation(0),
		m_active{true},
		m_color{WHITE},
		m_calculatedLocation{},
		m_width{},
		m_height{}
	{
	}

	Actor::~Actor()
	{
		//LOG("%s Actor deleted! ", m_id.c_str());
	}

	Vector2 Actor::GetWindowSize() const
	{
		return m_owningworld->GetAppWindowSize();
	}
	
	
	void Actor::SetLocation(Vector2 _location)
	{
		m_location = _location;
		m_calculatedLocation = _location;
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

	float Actor::GetScale()
	{
		return m_scale;
	}

	
	void Actor::Update(float _deltaTime)
	{
	}

	void Actor::Draw(float _deltaTime)
	{
	}

	void Actor::SetVisible(bool _flag)
	{
		m_visible = _flag;
	}

}
