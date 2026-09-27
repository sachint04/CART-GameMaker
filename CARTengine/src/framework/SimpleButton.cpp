#include <memory>
#include "SimpleButton.h"
#include "Application.h"
#include  "AssetManager.h"
#include "Core.h"
#include "component/InputController.h"
#include "World.h"
#include "HUD.h"
#include "UICanvas.h"
#include "Text.h"
namespace cart {
	
#pragma region  INIT
	
	
	SimpleButton::SimpleButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{ _owningworld, _id, isExcludedFromParentAutoControl },
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
		m_defaulttextcolor{BLACK},
		m_defaulttexturecolor{},
		m_textcolor{ BLACK },
		m_texthovercolor{BLUE},
		m_ButtonDefaultColor{},
		m_ButtonDownColor{},
		m_ButtonHoverColor{},
		m_IsButtonDown{ false },
		m_IsMouseOver{ false },
		m_IsSelected{ false },
		m_IsSelectable{ false },
		m_texturesourcedefault{},
		m_texturesourceover{},
		m_texturesourcedown{},
		m_texturesourcedisable{},
		m_ButtonDisableColor{GRAY},
		m_minfontsize{},
		m_cursorstyle{4},
		m_minfontspace{0.8f},
		m_btntxtprop{},
		m_renderedText{},
		m_renderedTextSize{}
	{
	}

	void SimpleButton::Init()
	{
		m_owningworld->GetInputController()->RegisterUI(GetWeakRef());
		UIElement::Init();
		
	}

	void SimpleButton::Start()
	{
		UIElement::Start();
	}

	void SimpleButton::SetScale(float _scale)
	{
		UIElement::SetScale(_scale);
	}

	

#pragma endregion

#pragma region LOOP
	
	void SimpleButton::Update(float _deltaTime)
	{
		if (!m_visible)return;
		if (m_fontstr.size() > 0) {
			Rectangle rect = GetBounds();
			float fsize = m_fontsize;
			float fspace = m_fontspace;			
			fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));					
			fspace = std::max(m_minfontspace, m_fontspace * World::UI_CANVAS.get()->Scale());
	
			m_font = AssetManager::Get().LoadFontAsset(m_fontstr, fsize);
			m_renderedText = Text::GetElidedText(*m_font, m_text, rect.width, fsize, fspace,2.f, m_renderedTextSize);
		}
		if (!m_active)return;


		UIElement::Update(_deltaTime);
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


			Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };
			bool mouseonBtn =  m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());
			if (mouseonBtn) {//Mouse over
				if (m_touch) // Mouse/touch active
				{
					if (IsMouseButtonReleased(0)) { // Mouse/Touch Released
						ButtonUp(tPos);
						m_touch = false;
					}else if (IsMouseButtonDown(0)) {// Dragging		
						m_owningworld->GetInputController()->SetFocus(GetId());
						ButtonDrag(tPos);
					}
				}
				else {
					if (IsMouseButtonPressed(0)) {// Pressed once
						ButtonDown(tPos);// Mouse /Touch  Pressed
						m_touch = true;
					}else if (IsMouseButtonUp(0)) {// Mouse over the button //
						MouseHovered();
					}
				}
			}
			else {
				if (m_touch) { // Mouse is out of bound while dragging
					if (tPos.x < 0)tPos.x = 0;
					if (tPos.x > GetScreenWidth()) tPos.x = GetScreenWidth();
					if (tPos.y < 0)tPos.y = 0;
					if (tPos.y > GetScreenHeight()) tPos.y = GetScreenHeight();

					if (IsMouseButtonReleased(0)) {// Cursor is out fo screen
						ButtonUp(tPos);
						m_touch = false;
					}
					else {
						ButtonDrag(tPos);// keep Dragging
					}
				}
				if (m_IsMouseOver) {
					MouseOut();
				}
				/*if (m_IsButtonDown) {
					ButtonUp(tPos);
				}*/
				
			}

	
#endif
	}

	void SimpleButton::Draw(float  _deltaTime)
	{
		if (!m_visible)return;
		
		UIElement::Draw(_deltaTime);
		Rectangle rect = GetBounds();
		

			if (m_text.size() > 0) {
				float fsize = m_fontsize;
				float fspace = m_fontspace;				
				fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
				fspace = std::max(m_minfontspace, m_fontspace * World::UI_CANVAS.get()->Scale());
				int padding = 2;
				int avaliableW = rect.width - (padding * 2);
				int avaliableH = rect.height -( padding * 2);
				if (m_renderedTextSize.y < avaliableH) {
					float x = rect.x + padding + (avaliableW - m_renderedTextSize.x) * 0.5f ;
					float y = rect.y + padding + (avaliableH - m_renderedTextSize.y) * 0.5f;
					m_font = AssetManager::Get().LoadFontAsset(m_fontstr, fsize);
					DrawTextEx(*m_font, m_renderedText.c_str(), {x, y}, fsize, fspace, m_textcolor);
				}
				
			}

	}


#pragma endregion
	
#pragma region Helpers
	
	void SimpleButton::SetSelected(bool _flag)
	{
		m_IsSelected = _flag;
		m_textcolor = m_IsSelected ? m_texthovercolor : m_defaulttextcolor;
	}

	void SimpleButton::SetActive(bool _flag)
	{
		UIElement::SetActive(_flag);		
		if (!_flag) {			
			MouseOut();			
			m_IsSelected = false;
			m_touch = false;
		}
		// TO DO
		Color textDisabledColor =  { m_defaulttextcolor.r, m_defaulttextcolor.g, m_defaulttextcolor.b, 200 };
		// == END
		m_color = (_flag)?m_ButtonDefaultColor : m_ButtonDisableColor;
		m_textcolor = (_flag) ? m_defaulttextcolor : textDisabledColor;
	}

	void SimpleButton::SetButtonProperties(Btn_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
		SetColor(_prop.btncol);
		m_ButtonDefaultColor = _prop.btncol;
		m_ButtonHoverColor = _prop.overcol;
		m_ButtonDownColor = _prop.downcol;
		m_ButtonDisableColor = _prop.disablecol;
		m_IsSelectable = _prop.selectable;
		m_defaulttexturecolor = _prop.textureColor;
		m_cursorstyle = _prop.cursorstyle;

	}
	void SimpleButton::SetButtonProperties(Btn_Text_Properties _prop)
	{
		//UIElement::SetUIProperties(_prop);
		SetTextProperties(_prop);	
		m_IsSelectable = _prop.selectable;

	}

	void SimpleButton::SetUIProperties(UI_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
	}

	void SimpleButton::SetTextProperties(Btn_Text_Properties _prop)
	{
		SetButtonProperties((Btn_Properties)_prop);


		m_btntxtprop.font = _prop.font;
		m_btntxtprop.text = _prop.text;
		m_btntxtprop.textcolor = _prop.textcolor;
		m_btntxtprop.color = {0,0,0,0};
		m_btntxtprop.align = CENTER;
		m_btntxtprop.valign = MIDDLE;
		m_btntxtprop.anchor = {0.5f,0.5f,0.5f,0.5f };
		m_btntxtprop.pivot = { 0.5f,0.5f };
		m_btntxtprop.size = _prop.size;
		m_btntxtprop.location = { 0,0 };
		m_btntxtprop.scale = _prop.scale;
		m_btntxtprop.fontsize = _prop.fontsize;
		m_btntxtprop.minfontsize = _prop.minfontsize;
		m_btntxtprop.fontspacing = _prop.fontspacing;
		m_btntxtprop.minfontspacing = _prop.minfontspace;
		m_btntxtprop.component = _prop.component;

		

		m_fontstr = _prop.font;
		m_text = _prop.text;
		m_fontsize = _prop.fontsize;
		m_minfontsize = _prop.minfontsize;
		m_align = _prop.textAlign;
		m_textcolor = _prop.textcolor;
		m_defaulttextcolor = _prop.textcolor;
		m_texthovercolor = _prop.texthoverolor;
		m_fontspace = _prop.fontspacing;
		m_minfontspace = _prop.minfontspace;


	}


	void SimpleButton::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
		
	}

	void SimpleButton::SetColor(Color _color)
	{
		m_ButtonDefaultColor = _color;
		m_color = _color;
	}
	
	void SimpleButton::SetHoverColor(Color _color)
	{
		m_ButtonHoverColor = _color;
	}
	
	void SimpleButton::SetDownColor(Color _color)
	{
		m_ButtonDownColor = _color;
	}

	void SimpleButton::SetDisableColor(Color _color)
	{
		m_ButtonDisableColor = _color;
	}

	void SimpleButton::SetFontName(const std::string &strfnt)
	{
		m_fontstr = strfnt;
	}


	bool SimpleButton::TestMouseOver(Vector2 _point)
	{
		return CheckCollisionPointRec(_point, GetBounds());
	}

	
	

#pragma endregion

#pragma region  UI EVENTS
	void SimpleButton::ButtonUp(Vector2 pos)
	{
		m_IsButtonDown = false;
		m_color = m_ButtonDefaultColor;		

		onButtonUp.Broadcast(GetWeakRef(),  pos);
	}
	
	void SimpleButton::ButtonDown(Vector2 pos)
	{
		m_IsButtonDown = true;		
		m_color = m_ButtonDownColor;
		
		if (m_IsSelectable == true) {
			m_IsSelected = true;
		}
		onButtonDown.Broadcast(GetWeakRef(), pos);

	}


	void SimpleButton::ButtonDrag(Vector2 pos) {

		onButtonDrag.Broadcast(GetWeakRef(), pos);
	}


	void SimpleButton::MouseHovered()
	{
		
		m_color = m_ButtonHoverColor;
		m_textcolor = m_texthovercolor;		
		
		m_IsMouseOver = true;
		SetMouseCursor(m_cursorstyle);
		onButtonHover.Broadcast(GetWeakRef() );


	}
	void SimpleButton::MouseOut()
	{
		//if(m_IsMouseOver == true){
			m_color = m_ButtonDefaultColor;
			m_textcolor =  m_IsSelected ? m_texthovercolor : m_defaulttextcolor;

			m_IsMouseOver = false;
			SetMouseCursor(0);

			m_IsButtonDown = false;
			//SetMouseCursor(MOUSE_CURSOR_ARROW);
			if (m_active) {
				onButtonOut.Broadcast(GetWeakRef() );
			}
		//}
	}



	
#pragma endregion

#pragma region BUTTON STATE
	
	/*Rectangle SimpleButton::GetBounds() 
	{	
		return { m_location.x, m_location.y, (float)m_width, (float)m_height };
	}*/

#pragma endregion

#pragma region CleanUp

	void SimpleButton::Destroy() {
		if (m_isPendingDestroy)return;
		m_font.reset();
		SetMouseCursor(0);
		m_owningworld->GetInputController()->RemoveUI(GetId());

		onButtonClicked.Destroy();
		onButtonDown.Destroy();
		onButtonUp.Destroy();
		onButtonDrag.Destroy();
		onButtonHover.Destroy();
		onButtonOut.Destroy();


		UIElement::Destroy();
	}


	SimpleButton::~SimpleButton()
	{

	
	}
#pragma endregion
}