#include "UIButton.h"
#include  "AssetManager.h"
#include "Core.h"

namespace cart {
	
#pragma region  INIT
	
	UIButton::UIButton(World* _owningworld, const std::string& _id) 
		:UIElement{ _owningworld, _id },
		m_touch{ false },
		tCount(0),
		m_margin(0),
		m_textsize{},
		m_fontspace{},
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
		m_IsButtonDown{ false },
		m_IsMouseOver{ false },
		m_IsSelected{ false },
		m_IsSelectable{ false }
	{

	};

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
		UpdateTextLocation();
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
					ButtonDown(tPos);

				}
				m_touch = true;
			}

			if (IsMouseButtonReleased(0)) {

				if (m_touch == false)return;

				if (TestMouseOver(tPos) == true) {
					ButtonUp(tPos);
				}
				
				m_touch = false;
			}
		
			if (m_IsMouseOver && m_IsButtonDown) {
					ButtonDrag(tPos);
				
			}
		}
#endif
	}

	void UIButton::Draw(float  _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);
		if (m_IsSelected == true) {
			DrawRectangle(m_calculatedLocation.x - 10.f, m_calculatedLocation.y - 10.f, m_width + 20.f, m_height + 20.f, m_color);
		}
		if (m_text.size() > 0) {
			shared<Font>m_font = AssetManager::Get().LoadFontAsset(m_fontstr, m_fontsize);
			Color calcColor = { m_textcolor.r, m_textcolor.g, m_textcolor.b, m_color.a };
			DrawTextEx(*m_font, m_text.c_str(), m_fontLocation, m_fontsize * m_scale, m_fontspace * m_scale, calcColor);
		}
		else {
			DrawRectangle(m_calculatedLocation.x, m_calculatedLocation.y, m_width , m_height, m_color);
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
	
	void UIButton::SetButtonProperties(Btn_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
		SetColor(_prop.btncol);
		m_ButtonDefaultColor = _prop.btncol;
		m_ButtonHoverColor = _prop.overcol;
		m_ButtonDownColor = _prop.downcol;
		m_IsSelectable = _prop.isSelectable;


	}
	void UIButton::SetButtonProperties(Btn_Text_Properties _prop)
	{
		SetTextProperties(_prop);	
		m_IsSelectable = _prop.isSelectable;

	}

	void UIButton::SetUIProperties(UI_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
	}

	void UIButton::SetTextProperties(Btn_Text_Properties _prop)
	{
		SetUIProperties(_prop);
		SetButtonProperties((Btn_Properties)_prop);
		m_fontstr = _prop.font;
		m_text = _prop.text;
		m_fontsize = _prop.fontsize;
		m_align = _prop.textAlign;
		m_textcolor = _prop.textcolor;
		m_defaulttextcolor = _prop.textcolor;
		m_texthovercolor = _prop.texthoverolor;
		m_fontspace = _prop.fontspace;
		//UpdateLocation();
     //   m_ButtonDefaultColor = _prop._color;

	}

	void UIButton::UpdateLocation()
	{
		UIElement::UpdateLocation();

	}

	void UIButton::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
		UpdateTextLocation();
	}

	void UIButton::UpdateTextLocation() {
		if (m_text.size() == 0)return;
		if (m_textsize.x < m_width) {
			float margin_x = ((m_width *  m_scale) - (m_textsize.x * m_scale))* 0.5f;
			float margin_y = ((m_height * m_scale) - (m_textsize.y * m_scale)) * 0.5f;

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
	void UIButton::ButtonUp(Vector2 pos)
	{
		m_IsButtonDown = false;
		m_color = m_ButtonDefaultColor;
		onButtonUp.Broadcast(GetWeakRef(),  pos);
		//LOG("UIBUTTON  RELEASE!!");
	}
	
	void UIButton::ButtonDown(Vector2 pos)
	{
		m_IsButtonDown = true;
		m_color = m_ButtonDownColor;
		if (m_IsSelectable == true) {
			m_IsSelected = true;
		}
		onButtonDown.Broadcast(GetWeakRef(), pos);
		//LOG("UIBUTTON  DOWN!!");
	}


	void UIButton::ButtonDrag(Vector2 pos) {
		//LOG("button dragging");
		onButtonDrag.Broadcast(GetWeakRef(), pos);
	}


	void UIButton::MouseHovered()
	{
		m_color = m_ButtonHoverColor;
		m_textcolor = m_texthovercolor;
		m_IsMouseOver = true;
		SetMouseCursor(1);
		onButtonHover.Broadcast(GetWeakRef() );
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
			m_IsButtonDown = false;
			onButtonOut.Broadcast(GetWeakRef() );
		}
	}

	
#pragma endregion

#pragma region BUTTON STATE
	
	Rectangle UIButton::GetBounds() 
	{	
		return { m_calculatedLocation.x, m_calculatedLocation.y, (float)m_width, (float)m_height };
	}

#pragma endregion

UIButton::~UIButton()
	{
		//LOG("%s UIButton Deleted ", m_id.c_str());
	}
}