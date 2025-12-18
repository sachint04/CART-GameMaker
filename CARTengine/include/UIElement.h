#pragma once
#include <string>
#include "Actor.h"
#include "Core.h"
#include "Types.h"
#include "component/LayoutComponent.h"



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
		virtual void SetPivot(Vector2 _pivot);
		virtual void Offset(Vector2 _location)override;
		virtual void SetAnchor(Rectangle rect);
		virtual void DrawBGColor();
		virtual void SetVisible(bool _flag) override;
		virtual void Notify(const std::string& strevent);
		virtual void AssetsLoadCompleted()override;
		virtual void UpdateLayout(int canvas_w, int canvas_h);
		virtual bool HasTexture();
		virtual TEXTURE_TYPE GetTextureType();
		virtual Rectangle GetBounds();
		virtual Vector2 GetPivot();
		virtual Rectangle GetAnchor();

		virtual void AddComponent(COMPONENT_TYPE type);
		weak<LayoutComponent> GetLayoutComponent();


		virtual ~UIElement();

		void AddText(const std::string & id, Text_Properties _txt);
		weak<UIButton> AddButton(const std::string& id, Btn_Text_Properties _btn);
		void AddChild(weak<UIElement> _btn);
		void RemoveChild(const std::string& id);
		bool IsPendingUpdate() { return m_pendingUpdate; };
		void SetPendingUpdate(bool _flag);
	
		void Destroy()override;
		void SetExcludeFromParentAutoControl(bool _flag);
		bool IsExcludedFromParentAutoControl() { return m_isExcludedFromParentAutoControl; };
		
		virtual void OnChildReady(const std::string& id);
		virtual void OnScreenSizeChange();
		virtual void OnLayoutChange();
		weak<UIElement> parent();
		void parent(weak<UIElement> ui);
	protected:
		SHAPE_TYPE m_shapeType;
		int m_borderwidth;
		int m_roundnessSegments;
		bool m_pendingUpdate;
		bool m_isExcludedFromParentAutoControl;
		float m_roundness;
		Vector2 m_rawlocation;	
		Vector2 m_pivot;
		Vector2 m_defaultSize;
		Rectangle m_anchor;
		shared<LayoutComponent> m_layout;
		weak<UIElement> m_parent;
		Color m_borderColor;
		std::vector <shared<UIButton>> m_slides = {};
		std::vector<shared<UIElement>> m_children = {};
		TEXTURE_TYPE  m_texturetype = TEXTURE_FULL;


	};

}