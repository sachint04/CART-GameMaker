#pragma once
#include <raylib.h>
#include "Delegate.h"
#include "UIElement.h"
#include "Types.h"

namespace cart {
  

	class UIButton : public UIElement {
	public:
        UIButton(World* _owningworld, const std::string& _id, Vector2 _size );
        void SetTextProperties(Btn_Text_Properties _prop);

        bool TestMouseOver(Vector2 _point);
        void ButtonUp();
        void ButtonDown();
        void MouseHovered();     
        void MouseOut();
        void SetButtonProperties(Btn_Properties _prop);


        Delegate<> onButtonClicked;
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
        void UpdateTextLocation();
		~UIButton();
        
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
        Color m_defaulttextcolor;
        Color m_textcolor;
        Color m_texthovercolor;
        Color m_ButtonDefaultColor;
        Color m_ButtonDownColor;
        Color m_ButtonHoverColor;
        bool m_IsButtonDown;
        bool m_IsMouseOver;
        bool m_IsSelected;
        bool m_IsSelectable;
      
     
	};

}