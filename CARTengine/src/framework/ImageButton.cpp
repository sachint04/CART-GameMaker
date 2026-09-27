#include <memory>
#include "ImageButton.h"
#include "AssetManager.h"
#include "Core.h"
#include "component/InputController.h"
#include "World.h"
namespace cart {
	
#pragma region  INIT
	ImageButton::ImageButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:Sprite2D{ _owningworld, _id, isExcludedFromParentAutoControl },
		m_touch{ false },
		tCount(0),
		m_margin(0),
		m_textsize{},
		m_fontspace{ 2.f },
		m_fontLocation{},
		m_locmouse{},
		m_fontstr{},
		m_text{},
		m_fontsize(),
		m_defaulttextcolor{},
		m_defaulttexturecolor{},
		m_textcolor{},
		m_texthovercolor{},
		m_ButtonDefaultColor{},
		m_ButtonDownColor{},
		m_ButtonHoverColor{},
		m_ButtonDisableColor{},
		m_bButtonDown{ false },
		m_bMouseOver{ false },
		m_bSelected{ false },
		m_bSelectable{ false },
		m_bDraggable{false},
		m_borderwidth{ 0 },
		m_borderColor{ GRAY },
		m_texturesourcedefault{},
		m_texturesourceover{},
		m_texturesourcedown{},
		m_texturesourcedisable{},
		m_dragableboundry{},
		m_bMaintainOffset{true},
		m_offset{0,0},
		m_isDragging{false}
	{
	}

	void ImageButton::Init()
	{
		Sprite2D::Init();
		m_owningworld->GetInputController()->RegisterUI(GetWeakRef());

		if (m_text.size() > 0) {
			m_font = AssetManager::Get().LoadFontAsset(m_fontstr, m_fontsize);
			m_textsize = MeasureTextEx(*m_font, m_text.c_str(), m_fontsize, 2.f);
			UpdateTextLocation();
		}
		
	}

	void ImageButton::SetScale(float _scale)
	{
		Sprite2D::SetScale(_scale);
		UpdateTextLocation();
	}

#pragma endregion

#pragma region LOOP
	
	void ImageButton::Update(float _deltaTime)
	{

		if (!m_visible || m_pendingUpdate)return;

		Sprite2D::Update(_deltaTime);
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
		if (m_active) {
			Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };
			bool mouseonBtn = m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());

			if (mouseonBtn || (m_bButtonDown && m_bDraggable )) {//Mouse over
				if (m_touch) // Mouse/touch active
				{
				
					if (IsMouseButtonReleased(0)) { // Mouse/Touch Released
						ButtonUp(tPos);
						m_touch = false;
					}
					if (m_bButtonDown) {// Drading		
						ButtonDrag(tPos);					
					}
				}
				else {
					if (IsMouseButtonPressed(0)) {// Drading

						ButtonDown(tPos);// Mouse /Touch  Pressed
						m_touch = true;
					}
					if (IsMouseButtonUp(0)) {// Mouse over the button //
						MouseHovered();
					}
				}
			}
			else {

				if (m_bMouseOver) {
					if (!m_bButtonDown || !m_bDraggable ) {
						MouseOut();
					}
				}
				if (m_bButtonDown) {
					ButtonUp(tPos);
					m_touch = false;
				}
			}
		}
#endif
	}

	void ImageButton::Draw(float  _deltaTime)
	{
		if (!m_visible)return;
		
		Sprite2D::Draw(_deltaTime);
		Color calcColor = { m_textcolor.r, m_textcolor.g, m_textcolor.b, m_color.a };
		if (m_text.size() > 0) {
				m_font = AssetManager::Get().LoadFontAsset(m_fontstr, m_fontsize);			
				DrawTextEx(*m_font, m_text.c_str(), m_fontLocation, m_fontsize * m_scale, m_fontspace * m_scale, calcColor);			
		}

	}


	void ImageButton::SetSelected(bool _flag)
	{		
		m_bSelected = _flag;
		m_texturesource = m_bSelected ? m_texturesourceover :  m_texturesourcedefault;
	}

	void ImageButton::SetActive(bool _flag)
	{
		Sprite2D::SetActive(_flag);		
		if (!_flag) {
			MouseOut();
			m_bSelected = false;
			m_touch = false;
		}
	//	m_color = (_flag) ? m_ButtonDefaultColor : m_ButtonDisableColor;
	}



#pragma endregion
	
#pragma region Helpers
	
	void ImageButton::SetButtonProperties(Btn_Properties _prop)
	{
		Sprite2D::SetUIProperties(_prop);
		SetColor(_prop.btncol);
		m_ButtonDefaultColor = _prop.btncol;
		m_ButtonHoverColor = _prop.overcol;
		m_ButtonDownColor = _prop.downcol;
		m_ButtonDisableColor = _prop.disablecol;
		m_bSelectable = _prop.selectable;
		m_defaulttexturecolor = _prop.textureColor;
		m_borderwidth = _prop.borderwidth;
		m_borderColor = _prop.bordercol;
		m_texturesourcedefault = m_texturesource;
		m_texturesourceover = _prop.texturesourceover;
		m_texturesourcedown = _prop.texturesourcedown;
		m_texturesourcedisable = _prop.texturesourcedisable;
		m_bDraggable = _prop.dragable;
		m_dragableboundry = _prop.dragableboundry;
		m_bMaintainOffset = _prop.maintainoffset;

	}
	void ImageButton::SetButtonProperties(Btn_Text_Properties _prop)
	{
		SetTextProperties(_prop);	
		m_bSelectable = _prop.selectable;

	}

	void ImageButton::SetUIProperties(UI_Properties _prop)
	{
		Sprite2D::SetUIProperties(_prop);
	}

	void ImageButton::SetTextProperties(Btn_Text_Properties _prop)
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
		m_fontspace = _prop.fontspacing;
		//UpdateLocation();
     //   m_ButtonDefaultColor = _prop._color;

	}

	/*void ImageButton::UpdateLocation()
	{
		Sprite2D::UpdateLocation();

	}*/

	void ImageButton::SetLocation(Vector2 _location)
	{
		Sprite2D::SetLocation(_location);
		UpdateTextLocation();
	}

	void ImageButton::SetColor(Color _color)
	{
		m_ButtonDefaultColor = _color;
		//m_color = _color;
	}


	void ImageButton::UpdateTextLocation() {
		if (m_text.size() == 0)return;
		if (m_textsize.x < m_width) {
			float margin_x = ((m_width *  m_scale) - (m_textsize.x * m_scale))* 0.5f;
			float margin_y = ((m_height * m_scale) - (m_textsize.y * m_scale)) * 0.5f;

			m_fontLocation = { m_location.x + margin_x, m_location.y + margin_y  };
		}
		else {
			m_fontLocation = { m_location.x, m_location.y };
		}
	}

	void ImageButton::SetFontName(const std::string &strfnt)
	{
		m_fontstr = strfnt;
	}

	void ImageButton::EnableDrag(bool flag)
	{
		m_bDraggable = flag;
	}

	void ImageButton::SetDragBounds(Rectangle rect)
	{
		m_dragableboundry = rect;
	}


	bool ImageButton::TestMouseOver(Vector2 _point)
	{
		return CheckCollisionPointRec(_point, GetBounds());
	}

	
	

#pragma endregion

#pragma region  UI EVENTS
	void ImageButton::ButtonUp(Vector2 pos)
	{
		m_bButtonDown = false;
		//Logger::Get()->Trace(std::format("Button Up {} ", GetId()));
		if (m_strTexture.size() > 0)
			m_textureColor = m_defaulttexturecolor;
		else
			m_color = m_ButtonDefaultColor;
		
		m_offset = { 0,0 };
		m_isDragging = false;
		onButtonUp.Broadcast(GetWeakRef(),  pos);
		
	}
	
	void ImageButton::ButtonDown(Vector2 pos)
	{
		m_bButtonDown = true;
	//	Logger::Get()->Trace(std::format("Button Down {} ", GetId()));
		if (m_strTexture.size() > 0)
			m_textureColor = m_ButtonDownColor;
		else
			m_color = m_ButtonDownColor;
		

		if (m_texturetype == TEXTURE_PART) {
			if(!m_bSelected)
			m_texturesource = m_texturesourcedown;
			else
			m_texturesource = m_texturesourceover;
				
		}
		
		if (m_bSelectable == true) {
			m_bSelected = true;
		}
		if (m_bDraggable)
		{
			if (m_bMaintainOffset) {
				Rectangle rect = GetBounds();
				Vector2 p = { rect.x + rect.width * m_pivot.x, rect.y + rect.height * m_pivot.y };
				m_offset = { p.x - pos.x   , p.y - pos.y};
			}
		}
		m_owningworld->GetInputController()->SetFocus(GetId());
		onButtonDown.Broadcast(GetWeakRef(), pos);
	}


	void ImageButton::ButtonDrag(Vector2 pos) {
		
		if (m_bDraggable) {
			//Logger::Get()->Trace(std::format("Dragging {} ", GetId()));
			if (pos.x > m_dragableboundry.x && pos.x < m_dragableboundry.x + m_dragableboundry.width &&
				pos.y > m_dragableboundry.y && pos.y < m_dragableboundry.y + m_dragableboundry.height)
			{
				SetLocation({pos.x + m_offset.x, pos.y + m_offset.y});
			}
			onButtonDrag.Broadcast(GetWeakRef(), { pos.x + m_offset.x, pos.y + m_offset.y });
			m_isDragging = true;
		}
	}


	void ImageButton::MouseHovered()
	{


		if (m_strTexture.size() > 0)
			m_textureColor = m_ButtonHoverColor;
		else
			m_color = m_ButtonHoverColor;

		if(m_text.size() > 0)
			m_textcolor = m_texthovercolor;

		if (m_texturetype == TEXTURE_PART) {
			m_texturesource = m_texturesourceover;
		}
		m_bMouseOver = true;
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
		onButtonHover.Broadcast(GetWeakRef() );
	
	}
	void ImageButton::MouseOut()
	{
		if(m_bMouseOver == true){
			if (m_strTexture.size() > 0)
				m_textureColor = m_defaulttexturecolor;
			else
				m_color = m_ButtonDefaultColor;


			if (m_texturetype == TEXTURE_PART) {
				if(m_bSelected)
					m_texturesource = m_texturesourceover;
				else
				m_texturesource = m_texturesourcedefault;
			}
			if (m_text.size() > 0)
				m_textcolor = m_defaulttextcolor;

			
			m_bMouseOver = false;
			SetMouseCursor(0);
			SetMouseCursor(MOUSE_CURSOR_ARROW);
			onButtonOut.Broadcast(GetWeakRef() );
			m_isDragging = false;
		}

	}

	
#pragma endregion

#pragma region BUTTON STATE
	/*
	Rectangle ImageButton::GetBounds() 
	{	
		return { m_location.x, m_location.y, (float)m_width, (float)m_height };
	}
	*/

#pragma endregion

#pragma region Clean Up
	void ImageButton::Destroy()
	{
		if (m_isPendingDestroy)return;

		m_owningworld->GetInputController()->RemoveUI(GetId());

		onButtonClicked.Destroy();
		onButtonDown.Destroy();
		onButtonUp.Destroy();
		onButtonDrag.Destroy();
		onButtonHover.Destroy();
		onButtonOut.Destroy();

		Sprite2D::Destroy();
	}

#pragma endregion


ImageButton::~ImageButton()
	{
	SetMouseCursor(0);
	m_font.reset();
		
	}
}