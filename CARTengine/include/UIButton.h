#pragma once
#include "Delegate.h"
#include "Sprite2D.h"

namespace cart {

	class UIButton : public UIElement{
	public:
    
        UIButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);
        void SetTextProperties(Btn_Text_Properties _prop);

        bool TestMouseOver(Vector2 _point);
        void ButtonUp(Vector2 pos);
        void ButtonDown(Vector2 pos);
        void ButtonDrag(Vector2 pos);
        void MouseHovered();     
        void MouseOut();
        void SetButtonProperties(Btn_Properties _prop);
        void SetButtonProperties(Btn_Text_Properties _prop);


        Delegate<weak<Object>, Vector2> onButtonClicked;
        Delegate<weak<Object>,  Vector2> onButtonDown;
        Delegate<weak<Object>, Vector2> onButtonUp;
        Delegate<weak<Object>, Vector2> onButtonDrag;
        Delegate<weak<Object>> onButtonHover;
        Delegate<weak<Object>> onButtonOut;

        Rectangle GetBounds() override;
        void Init() override;
        void SetScale(float _scale) override;
        void SetUIProperties(UI_Properties _prop) override;
        void Update(float _deltaTime) override;
        void Draw(float _deltaTime) override;
        void SetSelected(bool _flag);
        void SetActive(bool _flag) override;
        void UpdateLocation() override;
        void SetLocation(Vector2 _location) override;
        void SetColor(Color _color)override;
        void UpdateTextLocation();
        void SetFontName(const std::string& strfnt);
		~UIButton();
        void Destroy()override;
    protected:
        bool m_touch;
        int tCount;
        
	private:
        int m_margin;
        Vector2 m_textsize;
        Vector2 m_fontLocation;
        Vector2 m_locmouse;
        std::string m_fontstr;
        std::string m_text;
        ALIGN m_align = LEFT;
        float m_fontsize;
        float m_fontspace;
        Color m_defaulttextcolor;
        Color m_defaulttexturecolor;
        Color m_textcolor;
        Color m_texthovercolor;
        Color m_ButtonDefaultColor;
        Color m_ButtonDownColor;
        Color m_ButtonHoverColor;
        bool m_IsButtonDown;
        bool m_IsMouseOver;
        bool m_IsSelected;
        bool m_IsSelectable;
        shared<Font> m_font;
        int m_borderwidth;
        Color m_borderColor;
        Rectangle m_texturesourcedefault;
        Rectangle m_texturesourceover;
        Rectangle m_texturesourcedown;
        Rectangle m_texturesourcedisable;

     
	};

}