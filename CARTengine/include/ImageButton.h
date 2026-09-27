#pragma once
#include "Delegate.h"
#include "Sprite2D.h"

namespace cart {
 
	class ImageButton : public Sprite2D
    {
	public:    
        ImageButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);
        void SetTextProperties(Btn_Text_Properties _prop);

        bool TestMouseOver(Vector2 _point);
       virtual void ButtonDown(Vector2 pos);
       virtual void SetButtonProperties(Btn_Properties _prop);
       virtual void SetButtonProperties(Btn_Text_Properties _prop);
       virtual void ButtonUp(Vector2 pos);
       virtual void ButtonDrag(Vector2 pos);
       virtual void MouseHovered();
       virtual void MouseOut();


        Delegate<weak<Object>, Vector2> onButtonClicked;
        Delegate<weak<Object>,  Vector2> onButtonDown;
        Delegate<weak<Object>, Vector2> onButtonUp;
        Delegate<weak<Object>, Vector2> onButtonDrag;
        Delegate<weak<Object>> onButtonHover;
        Delegate<weak<Object>> onButtonOut;

      //  Rectangle GetBounds() override;
        void Init() override;
        void SetScale(float _scale) override;
        void SetUIProperties(UI_Properties _prop) override;
        void Update(float _deltaTime) override;
        void Draw(float _deltaTime) override;
        void SetSelected(bool _flag);
        void SetActive(bool _flag) override;
      //  void UpdateLocation() override;
        void SetLocation(Vector2 _location) override;
        void SetColor(Color _color)override;
        void Destroy() override;
        void UpdateTextLocation();
        void SetFontName(const std::string& strfnt);
        void EnableDrag(bool flag);
        void SetDragBounds(Rectangle rect);
        bool IsDraggable() { return m_bDraggable; };
        bool IsDragging() { return m_isDragging; };
		~ImageButton();
        
    protected:
        bool m_touch;
        int tCount;
        Rectangle m_texturesourcedefault;
        Rectangle m_texturesourceover;
        Rectangle m_texturesourcedown;
        Rectangle m_texturesourcedisable;
        bool m_bButtonDown;
        bool m_bMouseOver;
        bool m_bSelected;
        bool m_bSelectable;
        bool m_bDraggable;
        bool m_isDragging;
        bool m_bMaintainOffset;
        int m_margin;
        float m_fontsize;
        float m_fontspace;
        float m_borderwidth;
        Vector2 m_textsize;
        Vector2 m_fontLocation;
        Vector2 m_locmouse;
        Vector2 m_offset;
        std::string m_fontstr;
        std::string m_text;
        ALIGN m_align = LEFT;
        Color m_defaulttextcolor;
        Color m_defaulttexturecolor;
        Color m_textcolor;
        Color m_texthovercolor;
        Color m_ButtonDefaultColor;
        Color m_ButtonDownColor;
        Color m_ButtonHoverColor;
        Color m_ButtonDisableColor;
        Color m_borderColor;
        shared<Font> m_font;
        Rectangle m_dragableboundry;

     
	};

}