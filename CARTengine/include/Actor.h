#pragma once

#include <functional>
#include <vector>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include "Clock.h"
#include "Object.h"
#include "Delegate.h"
#include "Core.h"
#include "Types.h"

using json = nlohmann::json;

namespace cart {
	class World;
	class IComponent;
	class Material;
	class Actor : public Object {

	public:
		Delegate<const std::string&> onReady;

		struct DelayedTask {
			std::function<void()> func;
			std::chrono::steady_clock::time_point targetTime;
		};
		

		Actor( World* _owingworld, const std::string& _id );
		virtual ~Actor();
		virtual void Init();
		virtual void Start();
		virtual void Update(float _deltaTime);
		virtual void Draw(float _deltaTime);
		virtual void Destroy()override;
		virtual void SetLocation(Vector2 _location);
		virtual void SetLocation(Vector3 _location);
		virtual void Offset(Vector2 _location);
		virtual void SetScale(float _scale);
		virtual void SetScale3(Vector3 _scale);
		virtual void SetRotation(float _rotation);	
		virtual void SetRotation3(Vector4 _rotation);
		virtual void SetVisible(bool _flag);
		virtual bool IsVisible() const;
		virtual bool IsActive() const;
		virtual void SetActive(bool _flag);
		virtual void SetColor(Color _color);
		virtual void LateUpdate(float _deltaTime);
		virtual void SetSize(Vector2 _size);
		virtual void UpdateRawSize(Vector2 _size);
		virtual void SetSize(Vector3 _size);
		virtual Vector2 GetPadding() { return m_padding; };
		virtual void LoadAssets_async();
		virtual void SetReady(bool flag);
		virtual void AssetsLoadCompleted();
		virtual std::string type();
		virtual void SetTweening(bool flag);
		virtual bool IsTweening() { return m_tweening; };
		virtual weak<Object> GetParent()override;
		virtual weak<Object> SortChildrenByZindex()override;
		virtual bool AddChild(weak<Actor> elem);
		virtual weak<Object> Child(const std::string& id);
		virtual void SetZindex(int index, bool sort= true);
		virtual bool IsReady()override { return m_isReady; };
		void SetCustomData(json data);
		json& GetCustomData() {
			return m_customData;
		};

		float GetZindex() { return m_zIndex; };

		void SetMaterial(weak<Material> mat);
		weak<Material> GetMaterial();

		template<typename ActorType>
		void SetParent(ActorType actor);

		bool IsScaleLocked() { return m_isLockedScale; };
		float GetScale();
		float GetRotation();
		Color GetColor();
		Vector2 GetLocation();
		Vector3 GetLocation3();
		Vector4 GetRotation3();
		Vector2 GetSize();
		Vector3 GetSize3();
		Vector2 GetRawSize();
		Vector2 GetWindowSize() const;
		World* m_owningworld;

	protected:
		bool m_visible;
		bool m_active;
		bool m_isLockedScale;
		bool m_areAssetsLoaded;
		bool m_areChildrenReady;
		bool m_isReady;
		bool m_tweening;
		float m_zIndex;
		
		float m_width;
		float m_height;
		float m_zSize;
		float m_scale;
		float m_rotation;
		float m_rawWidth;
		float m_rawHeight;
		Vector2 m_location;
		Vector2 m_padding;
		Vector3 m_location3;
		Vector4 m_rotation3;
		Color m_color;
		std::vector <External_Asset> m_preloadlist; //<url, filename>
		std::string m_strloadMsg;

		json m_customData;

		//void DelayedTask(std::function<void()> func, int delay_ms);
		static void PostDelayed(std::function<void()> func, std::chrono::milliseconds delay);
		static inline std::vector<DelayedTask> m_tasks;
		weak<Material> m_material; // The "glue"
	};


	template<typename  ActorType>
	void Actor::SetParent(ActorType T)
	{
		m_parentObj = T;
	}
}