#pragma once
#include <string>
#include "Actor.h"
#include "Core.h"
#include "Types.h"
#include "component/IComponent.h"
#include "component/LayoutComponentFactory.h"



namespace cart {
	class World;
	class UIButton;
    class Text;
	class UIElement : public Actor  {
		
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
		virtual void SetDefaultLocation(Vector2 _location);
		virtual void SetPivot(Vector2 _pivot);
		virtual void Offset(Vector2 _location)override;
		virtual void SetAnchor(Rectangle rect);
		virtual void SetVisible(bool _flag) override;
		virtual std::string type()override;

		virtual void DrawBGColor();
		virtual void Notify(const std::string& strevent);
		virtual void AssetsLoadCompleted()override;
		virtual bool HasTexture();
		virtual TEXTURE_TYPE GetTextureType();
		virtual Rectangle GetBounds();
		virtual Vector2 GetPivot();
		virtual Rectangle GetAnchor();
		virtual void SetFocused(bool _flag);
		virtual void AddUIComponent(Layout_Component_Type type, UI_Layout_Properties layout_props);
		virtual float GetDefaultWidth();
		virtual float GetDefaultHeight();
		virtual Vector2 GetRawLocation() { return m_rawlocation; };
		virtual void SetStyle(UI_Style _style);

		void SetLayoutLocation(Vector2 _loc);
		void SetLayoutSize(Vector2 _size);
		std::vector<weak<UIElement>>Children();
		bool UpdateLayout();
		bool IsLayoutUpdated();
		weak<UIElement> parent();


		virtual ~UIElement();
		virtual void OnChildReady(const std::string& id);
		virtual void OnScreenSizeChange();
		virtual void OnLayoutChange();

		void AddText(const std::string & id, Text_Properties _txt);
		void AddChild(weak<UIElement> _btn);
		void RemoveChild(const std::string& id);
		bool IsPendingUpdate() { return m_pendingUpdate; };
		void SetPendingUpdate(bool _flag);
		void parent(weak<UIElement> ui);
		void SetExcludeFromParentAutoControl(bool _flag);
		void Destroy()override;
		void MaintainAspectRatio(bool _flag);

		bool IsExcludedFromParentAutoControl() { return m_isExcludedFromParentAutoControl; };		
		bool HasComponents();
		bool HasLayoutComponent(Layout_Component_Type type);
		bool IsAspectRatio() { return m_bAspectRatio; };

		weak<UIButton> AddButton(const std::string& id, Btn_Text_Properties _btn);
		weak<IComponent> GetComponentById(const std::string& id);
	protected:
		bool m_pendingUpdate;
		bool m_isExcludedFromParentAutoControl;
		bool m_isFocused;
		bool m_bAspectRatio;
		int m_borderwidth;
		int m_roundnessSegments;
		float m_roundness;

		SHAPE_TYPE m_shapeType;
		Vector2 m_rawlocation;	
		Vector2 m_layoutlocation;	
		Vector2 m_pivot;
		Vector2 m_defaultSize;
		Vector2 m_layoutSize;
		Rectangle m_anchor;
	//	shared<IComponent> m_layout;
		weak<UIElement> m_parent;
		Color m_borderColor;
		std::vector <shared<UIButton>> m_slides = {};
		std::vector<shared<UIElement>> m_children = {};
		TEXTURE_TYPE  m_texturetype = TEXTURE_FULL;
		UI_Style m_style;
		LayoutComponentFactory m_ui_comp_factory;
	};

}