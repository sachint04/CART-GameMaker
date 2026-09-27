#pragma once
#include <string>
#include "Actor.h"
#include "Core.h"
#include "Types.h"
#include "Delegate.h"
#include "component/IComponent.h"
#include "component/LayoutComponentFactory.h"



namespace cart {
	class World;
	class UIButton;
	class ImageButton;
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
		virtual void LoadAssets_async()override;
		virtual void Offset(Vector2 _location)override;
		virtual bool IsUI() override;
		virtual void AssetsLoadCompleted()override;
		virtual weak<Object> SortChildrenByZindex()override;
		virtual std::string type()override;

		virtual void SetUIProperties(UI_Properties _prop);
        virtual void SetSize(Vector2 _size) override;		
		virtual void SetScale(float _scale) override;
		virtual void SetActive(bool _flag) override;
		virtual void SetLocation(Vector2 _location)override;		
		virtual void SetDefaultLocation(Vector2 _location);
		virtual void SetPivot(Vector2 _pivot);
		virtual void SetAnchor(Rectangle rect);
		virtual void SetVisible(bool _flag) override;
		virtual void SetZindex(int index, bool sort = true)override;

        virtual void SetDefaultSize();			
		virtual void DrawBGColor();
		virtual void Notify(const std::string& strevent);
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
		virtual void OnModalClose();
		

		virtual ~UIElement();
		virtual void OnChildReadyHandler(const std::string& id);
		virtual void OnScreenSizeChangeHandler();
		virtual void OnLayoutChangeHandler();
		
		virtual weak<UIButton> AddButton(const std::string& id, Btn_Text_Properties _btn);
		virtual weak<Object> Child(const std::string& id)override;
		virtual bool AddButtonElement(weak<UIButton> _btn);
		virtual bool AddButtonElement(weak<ImageButton> _btn);
		virtual bool AddChild(weak<Actor> elem) override;
		virtual weak<Text> AddText(const std::string & id, Text_Properties _txt);

		void SetLayoutLocation(Vector2 _loc);
		void SetLayoutSize(Vector2 _size);
		void RemoveChild(const std::string& id);
		void SetPendingUpdate(bool _flag);
		void SetExcludeFromParentAutoControl(bool _flag);
		void MaintainAspectRatio(bool _flag);
		bool UpdateLayout();
		bool IsLayoutUpdated();
		bool IsPendingUpdate() { return m_pendingUpdate; };
		bool IsExcludedFromParentAutoControl() { return m_isExcludedFromParentAutoControl; };		
		bool HasComponents();
		bool HasLayoutComponent(Layout_Component_Type type);
		bool IsAspectRatio() { return m_bAspectRatio; };
		weak<UIElement> GetUIParent();
		std::vector<weak<UIElement>>Children();
		
		void Destroy()override;

		weak<IComponent> GetComponentById(const std::string& id);

		Delegate<> onLayoutChanged;
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
		Color m_borderColor;
		std::vector <shared<UIButton>> m_slides = {};
		std::vector<shared<UIElement>> m_children = {};
		TEXTURE_TYPE  m_texturetype = TEXTURE_FULL;
		UI_Style m_style;
		LayoutComponentFactory m_ui_comp_factory;
		//weak<UIElement> m_parent;
	};



}