#pragma once
#include <raylib.h>
#include "Object.h"
namespace cart {
	class World;
	class Actor : public Object {

	public:
		Actor( World* _owingworld, const std::string& _id );
		virtual ~Actor();
		virtual void SetLocation(Vector2 _location);
		virtual void SetLocation(Vector3 _location);
		virtual void Offset(Vector2 _location);
		virtual void SetScale(float _scale);
		virtual void SetRotation(float _rotation);		
		virtual void SetVisible(bool _flag);
		virtual bool IsVisible() const;
		virtual bool IsActive() const;
		virtual void SetActive(bool _flag);
		virtual void SetColor(Color _color);
		virtual void Update(float _deltaTime);
		virtual void Draw(float _deltaTime);
		virtual void SetSize(Vector2 _size);
		virtual void SetSize(Vector3 _size);
		float GetRotation();
		Color GetColor();
		Vector2 GetLocation();
		Vector3 GetLocation3();
		float GetScale();
		Vector2 GetWindowSize() const;
		World* m_owningworld;
	protected:
		float m_width;
		float m_height;
		float m_zSize;
		float m_scale;
		float m_rotation;
		Vector2 m_location;
		Vector3 m_location3;
		Vector2 m_calculatedLocation;
		bool m_visible;
		bool m_active;
		Color m_color;
	};
}