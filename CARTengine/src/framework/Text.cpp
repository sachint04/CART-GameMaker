#include "Text.h"
#include "AssetManager.h"
#include "World.h"
#include "UICanvas.h"
namespace cart {

#pragma region  Constructor & Initialization
	Text::Text(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{ _owningworld,_id, isExcludedFromParentAutoControl },
		m_text{ },
		m_font{ },
		m_fontsize{ },
		m_margin(5),
		m_textsize{},
		m_align{ LEFT },
		m_background{ 0,0,0,0 },
		m_textColor{ BLACK },
		m_fontspacing{ 1.f },
		m_textLocation{},
		m_minfontsize{},
		m_minfontspacing{}
	{
	}

	void Text::Init()
	{
		UIElement::Init();
		
	}

#pragma endregion

#pragma region  LOOP

	void Text::Update(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, std::ceil(m_fontsize *  World::UI_CANVAS.get()->Scale()));
		m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), std::ceil(m_fontsize *  World::UI_CANVAS.get()->Scale()), 2.f *  World::UI_CANVAS.get()->Scale());
		UpdateTextLocation();
	}
	void Text::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);		
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
		float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());

		//if(!m_sharedfont)
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

		//m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), fsize, fspace);
		DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_background);	

		DrawTextEx(*m_sharedfont, m_text.c_str(), m_textLocation, fsize, fspace, m_textColor);
	}

	void Text::UpdateTextLocation()
	{				
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
		float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());

	//	if (!m_sharedfont)
			m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

		m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), fsize, fspace);
		int px = (m_pivot.x * m_width);
		int py = (m_pivot.y * m_height);
		Rectangle rect = GetBounds();
		switch (m_align)
		{
		case LEFT:
			m_textLocation = { rect.x - px,
								rect.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 - py } ;
			break;
		case CENTER:
			m_textLocation = { rect.x + ((rect.width - m_textsize.x) * 0.5f) * m_scale,
							rect.y + ((rect.height - m_textsize.y) * 0.5f) * m_scale };
			//m_textLocation = { m_location.x + (m_width * m_scale / 2) - (m_textsize.x * m_scale) / 2  -px,
			//					m_location.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 - py };
			break;

		case RIGHT:
			m_textLocation = { rect.x + (m_width * m_scale) - (m_textsize.x * m_scale) - px,
								rect.y + (m_height * m_scale / 2) - (m_textsize.y * m_scale) / 2 - py };
			break;
		case JUSTIFIED :
			break;
		}

	}
#pragma endregion


#pragma region  Helpers
	void Text::SetTextProperties(Text_Properties _prop)
	{	
		if (!ValidateTextProperties(_prop)) {
			// T B D
		}
		SetUIProperties((UI_Properties)_prop);
		m_font = _prop.font;
		m_text = _prop.text;
		m_fontsize = std::max(_prop.fontsize, _prop.minfontsize);
		m_minfontsize = _prop.minfontsize;
		m_align = _prop.align;	
		m_background = _prop.textbackground;
		m_fontspacing = _prop.fontspacing;
		m_minfontspacing = _prop.minfontspacing;
		m_textColor = _prop.textcolor;
		
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
		float tmpsize = std::max(size, m_minfontsize);
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, tmpsize);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_fontsize = std::max(size, m_minfontsize);
	}
	void Text::SetTextColor(Color col)
	{
		m_textColor = col;
	}
	void Text::UpdateText(const std::string& str) {
		m_text = str;
	}
	bool Text::ValidateTextProperties(Text_Properties _prop)
	{
		return _prop.fontsize && _prop.minfontsize && _prop.fontspacing && _prop.minfontspacing;
	}
#pragma endregion


#pragma region  Clean Up

	Text::~Text()
	{
		if (m_isPendingDestroy)return;

		m_sharedfont.reset();
	//	LOG("%s Text Deleted!", m_id.c_str());
	}

	void Text::Destroy() {
		UIElement::Destroy();
	}

#pragma endregion
}
