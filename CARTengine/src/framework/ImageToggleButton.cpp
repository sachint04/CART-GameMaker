#include "ImageToggleButton.h"
#include "Actor.h"
#include "Types.h"

namespace cart {
#pragma region Constructor & Init
	ImageToggleButton::ImageToggleButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl):
		ImageButton{ _owningworld, _id, isExcludedFromParentAutoControl }, m_bToggled{ false }, m_primaryRect{}, m_secondaryRect{}
	{
	}
	void ImageToggleButton::Init()
	{
		ImageButton::Init();
	}
#pragma endregion

#pragma region Helpers


	void ImageToggleButton::SetButtonProperties(Btn_Properties _prop)
	{
		ImageButton::SetButtonProperties(_prop);
		m_primaryTexture = _prop.primarytexture;
		m_secondaryTexture = _prop.secondarytexture;
		m_primaryRect = _prop.primarytogglerect;
		m_secondaryRect = _prop.secondarytogglerect;
	}
	void ImageToggleButton::SetButtonProperties(Btn_Text_Properties _prop)
	{
		ImageButton::SetButtonProperties(_prop);
	}
#pragma endregion

#pragma region Event Handlers
	
	void ImageToggleButton::ButtonUp(Vector2 pos)
	{
		Toggle();
		m_strTexture = m_bToggled ? m_secondaryTexture : m_primaryTexture;
		if (m_texturetype == TEXTURE_PART) {	
			if (m_bToggled) {
				m_texturesourcedefault = m_secondaryRect.defaultrect;
				m_texturesourceover = m_secondaryRect.hoverrect;
				m_texturesourcedown = m_secondaryRect.downrect;
				m_texturesourcedisable = m_secondaryRect.disabledrect;			
			}
			else {
				m_texturesourcedefault = m_primaryRect.defaultrect;
				m_texturesourceover = m_primaryRect.hoverrect;
				m_texturesourcedown = m_primaryRect.downrect;
				m_texturesourcedisable = m_primaryRect.disabledrect;
			}
		}
		ImageButton::ButtonUp(pos);
	}
	void ImageToggleButton::Toggle()
	{
		m_bToggled = !m_bToggled;
	}
#pragma endregion
#pragma region CleanUp
	void ImageToggleButton::Destroy()
	{
		if (IsPendingDestroy())return;

		ImageButton::Destroy();
	}
	ImageToggleButton::~ImageToggleButton()
	{
	}
#pragma endregion
}