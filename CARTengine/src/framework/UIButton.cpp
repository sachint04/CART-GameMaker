#include "UIButton.h"
#include  "AssetManager.h"
#include "Core.h"

namespace cart {
	
#pragma region  INIT
	

	UIButton::UIButton( World* _owningworld, const std::string& _id, Vector2 _size)
		:UIElement{ _owningworld, _id, _size },
		m_touch{false},
		tCount(0),
		m_margin(0),
		m_textsize{},
		m_fontLocation{},
		m_locmouse{},
		m_fontstr{},
		m_text{},
		m_align{},
		m_fontsize(),
		m_defaulttextcolor{},
		m_textcolor{},
		m_texthovercolor{},
		m_ButtonDefaultColor{},
		m_ButtonDownColor{},
		m_ButtonHoverColor{},
		m_IsButtonDown{false},
		m_IsMouseOver{ false },
		m_IsSelected{false},
		m_IsSelectable{false}
        {
		
		};

	void UIButton::Init()
	{
		shared<Font>m_font = AssetManager::Get().LoadFontAsset(m_fontstr, m_fontsize);
		UpdateLocation();
		if (m_text.size() > 0) {
			m_textsize = MeasureTextEx(*m_font, m_text.c_str(), m_fontsize, 2.f);
			UpdateTextLocation();
		}
		m_pendingUpdate = false;
	}

	void UIButton::SetScale(float _scale)
	{
		UIElement::SetScale(_scale);
		UpdateLocation();
		UpdateTextLocation();
	}

	void UIButton::SetButtonProperties(Btn_Properties _prop)
	{
		m_color = _prop.defaultcol;
		m_ButtonDefaultColor = _prop.defaultcol;
		m_ButtonHoverColor = _prop.overcol;
		m_ButtonDownColor = _prop.downcol;
		m_textcolor = _prop.textcolor;
		m_IsSelectable = _prop.isSelectable;

	}

	void UIButton::SetUIProperties(UI_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
	}

#pragma endregion

#pragma region LOOP
	
	void UIButton::Update(float _deltaTime)
	{
		if (m_visible == false)return;


#if defined(PLATFORM_ANDROID)
		tCount = GetTouchPointCount();
		/*
		Vector2 tData[10] = {};
		if (tCount > 0) {
			if (tCount > 10) tCount = 10;

			// Get touch points positions
			for (int i = 0; i < tCount; ++i) {
				tData[i] = GetTouchPosition(i);// MULTI TOUCH
			}
		}
		*/


		if (tCount > 0) {
			m_locmouse = GetTouchPosition(0);
            if (m_touch == false) {// NO TOUCH & NO CARD PICKED
                if (TestMouseOver(m_locmouse) == true) {
                    ButtonDown();
                }
            }
			m_touch = true;
		}
		else {

			if (m_touch == true ) {
				if (TestMouseOver(m_locmouse) == true) {
					ButtonUp();
				}
			}
			m_touch = false;
		}

#else
		if (m_active == true) {
			Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };

			if (IsMouseButtonUp(0) && m_touch == false) {
				if (TestMouseOver(tPos) == true) {
					MouseHovered();
				}
				else {
					MouseOut();
				}
			}

			if (IsMouseButtonPressed(0)) {
				if (m_touch == true)return;
				if (TestMouseOver(tPos) == true) {
					ButtonDown();

				}
				m_touch = true;
			}

			if (IsMouseButtonReleased(0)) {

				if (m_touch == false)return;

				if (TestMouseOver(tPos) == true) {
					ButtonUp();
				}

				m_touch = false;
			}
		}
#endif
	}

	void UIButton::Draw(float  _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);
		if (m_IsSelected == true) {
			DrawRectangle(m_calculatedLocation.x - 10.f, m_calculatedLocation.y - 10.f, m_size.x + 20.f, m_size.y + 20.f, m_color);
		}
		if (m_text.size() > 0) {
			shared<Font>m_font = AssetManager::Get().LoadFontAsset(m_fontstr, m_fontsize);
			Color calcColor = { m_textcolor.r, m_textcolor.g, m_textcolor.b, m_color.a };
			DrawTextEx(*m_font, m_text.c_str(), m_fontLocation, m_fontsize * m_scale, 2 * m_scale, calcColor);
		}
	}

	void UIButton::SetSelected(bool _flag)
	{
		m_IsSelected = _flag;
	}

	void UIButton::SetActive(bool _flag)
	{
		UIElement::SetActive(_flag);		
		if (_flag == false) {
			m_IsSelected = false;
		}
	}



#pragma endregion
	
#pragma region Helpers
	
	void UIButton::SetTextProperties(Btn_Text_Properties _prop)
	{
		SetUIProperties(_prop);
		SetButtonProperties(_prop);
		m_fontstr = _prop.font;
		m_text = _prop.text;
		m_fontsize = _prop.fontsize;
		m_align = _prop.align;
		m_textcolor = _prop.textcolor;
		m_defaulttextcolor = _prop.textcolor;
		m_texthovercolor = _prop.hovercolor;
		//UpdateLocation();
     //   m_ButtonDefaultColor = _prop._color;

	}

	void UIButton::UpdateLocation()
	{
		m_rawlocation = m_location;
			
		Vector2 updatedLoction = {};
		switch (m_align)
		{
		case LEFT:
			updatedLoction = { m_location.x ,  m_location.y - (m_size.y * m_scale)  / 2.f };
			
			break;
		case CENTER:
			updatedLoction = { m_location.x - (m_size.x * m_scale) / 2, m_location.y - (m_size.y * m_scale) / 2 };
			break;

		case RIGHT:
			updatedLoction = { m_location.x - (m_size.x * m_scale),  m_location.y - (m_size.y * m_scale) / 2 };
			break;
		}
		
		m_calculatedLocation = updatedLoction;

	}

	void UIButton::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
		UpdateTextLocation();
	}

	void UIButton::UpdateTextLocation() {
		if (m_text.size() == 0)return;
		if (m_textsize.x < m_size.x) {
			float margin_x = ((m_size.x *  m_scale) - (m_textsize.x * m_scale))* 0.5f;
			float margin_y = ((m_size.y * m_scale) - (m_textsize.y * m_scale)) * 0.5f;

			m_fontLocation = { m_calculatedLocation.x + margin_x, m_calculatedLocation.y + margin_y  };
		}
		else {
			m_fontLocation = { m_calculatedLocation.x, m_calculatedLocation.y };
		}
	}


	bool UIButton::TestMouseOver(Vector2 _point)
	{
		return CheckCollisionPointRec(_point, GetBounds());
	}

	
	

#pragma endregion

#pragma region  UI EVENTS
	void UIButton::ButtonUp()
	{
		m_IsButtonDown = false;
		m_color = m_ButtonDefaultColor;
		onButtonClicked.Broadcast();
		//LOG("UIBUTTON  RELEASE!!");
	}
	
	void UIButton::ButtonDown()
	{
		m_IsButtonDown = true;
		m_color = m_ButtonDownColor;
		if (m_IsSelectable == true) {
			m_IsSelected = true;
		}
		//LOG("UIBUTTON  DOWN!!");
	}
	
	void UIButton::MouseHovered()
	{
		m_color = m_ButtonHoverColor;
		m_textcolor = m_texthovercolor;
		m_IsMouseOver = true;
		SetMouseCursor(1);
	//	LOG("UIBUTTON  HOVER!!");
	}
	void UIButton::MouseOut()
	{
		if(m_IsMouseOver == true){
			m_color = m_ButtonDefaultColor;
			m_textcolor = m_defaulttextcolor;
			m_IsMouseOver = false;
			SetMouseCursor(0);
		//	LOG("UIBUTTON  OUT!!");
		}
	}

#pragma endregion

#pragma region BUTTON STATE
	
	Rectangle UIButton::GetBounds() 
	{	
		return { m_calculatedLocation.x, m_calculatedLocation.y, (float)m_size.x, (float)m_size.y };
	}

#pragma endregion

UIButton::~UIButton()
	{
		//LOG("%s UIButton Deleted ", m_id.c_str());
	}
}