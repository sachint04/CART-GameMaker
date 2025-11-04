#pragma once
#include <string>
#include "Actor.h"
#include "Core.h"
#include "Types.h"
namespace cart {
	class World;
	class UIButton;
    class Text;
	class UIElement : public Actor {
		
	public:
		
		UIElement(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);

		// UIElement virtual function
		virtual void Init() override;  
		virtual void Start() override;  
		virtual void Update(float _deltaTime) override;
		virtual void Draw(float _deltaTime) override;	
		virtual void LateUpdate(float _deltaTime) override;
		virtual void SetUIProperties(UI_Properties _prop);
        virtual void SetSize(Vector2 _size) override;		
		virtual void LoadAssets()override;

		virtual void SetScale(float _scale) override;
		virtual void SetActive(bool _flag) override;
		virtual void SetLocation(Vector2 _location)override;		
		virtual void SetPivot(Vector2 _pivot);
		virtual void Offset(Vector2 _location)override;
		virtual void DrawBGColor();
		virtual void SetVisible(bool _flag) override;
		virtual void Notify(const std::string& strevent);
		virtual void AssetsLoadCompleted()override;

		virtual Rectangle GetBounds();
		virtual ~UIElement();

		void AddText(const std::string & id, Text_Properties _txt);
		weak<UIButton> AddButton(const std::string& id, Btn_Text_Properties _btn);
		void AddChild(weak<UIElement> _btn);
		void RemoveChild(const std::string& id);
		bool IsPendingUpdate() { return m_pendingUpdate; };
		void SetPendingUpdate(bool _flag);
	
		Vector2 GetPivot();
		void Destroy()override;
		void SetExcludeFromParentAutoControl(bool _flag);
		bool IsExcludedFromParentAutoControl() { return m_isExcludedFromParentAutoControl; };
	
	protected:
		Vector2 m_rawlocation;	
		Vector2 m_pivot;
		std::vector<shared<UIElement>> m_children = {};
		std::vector <shared<UIButton>> m_slides = {};
		virtual void UpdateLocation();		
		bool m_pendingUpdate;
		Vector2 m_defaultSize;
		bool m_isExcludedFromParentAutoControl;
		SHAPE_TYPE m_shapeType;
		
	};

}