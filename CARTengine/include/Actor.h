#pragma once
#include <vector>
#include <raylib.h>
#include "Object.h"
namespace cart {
	class World;
	class Component;
	class Actor : public Object {

	public:
		Actor( World* _owingworld, const std::string& _id );
		virtual ~Actor();
		virtual void Start();
		virtual void SetLocation(Vector2 _location);
		virtual void SetLocation(Vector3 _location);
		virtual void Offset(Vector2 _location);
		virtual void SetScale(float _scale);
		virtual void SetRotation(float _rotation);	
		virtual void SetRotation3(Vector4 _rotation);
		virtual void SetVisible(bool _flag);
		virtual bool IsVisible() const;
		virtual bool IsActive() const;
		virtual void SetActive(bool _flag);
		virtual void SetColor(Color _color);
		virtual void Update(float _deltaTime);
		virtual void Draw(float _deltaTime);
		virtual void LateUpdate(float _deltaTime);
		virtual void SetSize(Vector2 _size);
		virtual void SetSize(Vector3 _size);
		virtual void LoadAssets();
		virtual void Init();
		virtual void SetReady(bool flag);
		virtual void AddComponent(Component& component);

		virtual void AssetsLoadCompleted();
		bool IsReady() { return m_isReady; };

		float GetRotation();
		Color GetColor();
		Vector2 GetLocation();
		Vector3 GetLocation3();
		Vector4 GetRotation3();
		float GetScale();
		Vector2 GetSize();
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
		Vector4 m_rotation3;
		Vector2 m_calculatedLocation;
		bool m_visible;
		bool m_active;
		Color m_color;
		bool m_isReady;
		std::vector <std::string> m_preloadlist; //<url, filename>
		std::string m_strloadMsg;
		Dictionary<unsigned char, Component&> componentlist;
	};
}