#include "Text.h"
#include "AssetManager.h"
#include "World.h"
namespace cart {

#pragma region  Constructor & Initialization
	Text::Text(World* _owningworld, const std::string& _id)
		:UIElement{ _owningworld,_id },
		m_text{ },
		m_font{ },
		m_fontsize{ },
		m_margin(5),
		m_textsize{},
		m_align{ LEFT },
		m_background{ 0,0,0,0 },
		m_textColor{ BLACK },
		m_fontspacing{ 1.f },
		m_textLocation{}
	{
	}

	void Text::Init()
	{
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
		
		UIElement::Init();
		
	}

#pragma endregion

#pragma region  LOOP
	void Text::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);
		if (!m_sharedfont) {
			m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
		}
		m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), m_fontsize, m_fontspacing);
		DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_background);	

		DrawTextEx(*m_sharedfont, m_text.c_str(), m_textLocation, m_fontsize * m_scale, 1, m_textColor);
	}

	void Text::UpdateLocation()
	{
		UIElement::UpdateLocation();
		if (!m_sharedfont) {
			m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
		}
		m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), m_fontsize, m_fontspacing);
		switch (m_align)
		{
		case LEFT:
			m_textLocation = { m_calculatedLocation.x,
								m_calculatedLocation.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 };
			break;
		case CENTER:
			m_textLocation = { m_calculatedLocation.x + (m_width * m_scale / 2) - (m_textsize.x * m_scale) / 2, 
								m_calculatedLocation.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 };
			break;

		case RIGHT:
			m_textLocation = { m_calculatedLocation.x + (m_width * m_scale) - (m_textsize.x * m_scale), 
								m_calculatedLocation.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 };
			break;
		}

	}
#pragma endregion


#pragma region  Helpers
	void Text::SetTextProperties(Text_Properties _prop)
	{
		m_font = _prop.font;
		m_text = _prop.text;
		m_fontsize = _prop.fontsize;
		m_align = _prop.align;	
		m_background = _prop.textbackground;
		m_fontspacing = _prop.fontspacing;
		m_textColor = _prop.textcolor;
		SetUIProperties((UI_Properties)_prop);
	}
	void Text::SetFontName(const std::string& strfnt)
	{
		m_sharedfont = AssetManager::Get().LoadFontAsset(strfnt, m_fontsize);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_font = strfnt;
	}

	std::string Text::GetFontName() {
		return m_font;
	}
	void Text::SetFontSize(float size)
	{
		if (m_fontsize == size)return;
		m_sharedfont.reset();
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, size);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_fontsize = size;
		UpdateLocation();

	}
	void Text::SetTextColor(Color col)
	{
		m_textColor = col;
	}

	void Text::UpdateText(const std::string& str) {
		m_text = str;
		UpdateLocation();
	}
#pragma endregion


#pragma region  Clean Up

	Text::~Text()
	{
		m_sharedfont.reset();
	//	LOG("%s Text Deleted!", m_id.c_str());
	}

	void Text::Destroy() {
		UIElement::Destroy();
	}

#pragma endregion
}
