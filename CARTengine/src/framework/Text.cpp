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
		m_margin(10),
		m_align{ LEFT },
		m_background{ 0,0,0,0 },
		m_textColor{ BLACK },
		m_fontspacing{ 1.f },
		m_textLocation{},
		m_minfontsize{},
		m_maxfontsize{},
		m_minfontspacing{},
		m_valign{MIDDLE},
		m_multiline{false},
		m_strlines{}
	{
	}

	void Text::Init()
	{
		UIElement::Init();
		
	}

	void Text::Start()
	{
		UIElement::Start();

	}

#pragma endregion

#pragma region  LOOP

	void Text::Update(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, std::ceil(m_fontsize *  World::UI_CANVAS.get()->Scale()));
		UpdateTextLocation();
		
	}
	void Text::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);		
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
		float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
		Rectangle rect = GetBounds();
		//if(!m_sharedfont)
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

		//m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), fsize, fspace);
		DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_background);	

		if (m_multiline) {
			for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
			{
				DrawTextEx(*m_sharedfont, iter->first.c_str(), iter->second, fsize, fspace, m_textColor);
			}
		}
		else {
			DrawTextEx(*m_sharedfont, m_strlines.begin()->first.c_str(), m_strlines.begin()->second, fsize, fspace, m_textColor);
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
		m_maxfontsize = _prop.maxfontsize;
		m_align = _prop.align;	
		m_valign = _prop.valign;	
		m_background = _prop.textbackground;
		m_fontspacing = _prop.fontspacing;
		m_minfontspacing = _prop.minfontspacing;
		m_textColor = _prop.textcolor;	
		m_multiline = _prop.multiline;
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
		
		if (m_maxfontsize > 0) tmpsize = std::min(m_maxfontsize, tmpsize);

		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, tmpsize);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_fontsize = tmpsize;// std::max(size, m_minfontsize);
	}
	void Text::SetTextColor(Color col)
	{
		m_textColor = col;
	}
	void Text::SetText(const std::string& str) {
		m_text = str;
	}
	void Text::SetAligned(ALIGN _align)
	{
		m_align = _align;
	}
	void Text::SetVAligned(V_ALIGN _valign)
	{
		m_valign = _valign;
	}
	bool Text::ValidateTextProperties(Text_Properties _prop)
	{
		return _prop.fontsize && _prop.minfontsize && _prop.fontspacing && _prop.minfontspacing;
	}
	void Text::UpdateTextLocation()
	{
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
		float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
		Rectangle rect = GetBounds();
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

#pragma region  Draw Text
		m_strlines = {};
		std::string strcopy = m_text;
		std::string spacedelimiter = " ";
		int theight = 0;
		std::string line = "";
		float linespacing = 2.0f;
		Vector2 msize = { 0,0 };
		float msgheight = 0;
		float maxwidth = rect.width - (m_margin * 2);
		while (strcopy.find(spacedelimiter) != std::string::npos) {// create stings of line in
			auto find = strcopy.find(spacedelimiter);			
			line = line + strcopy.substr(0, find + 1);
			msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
			if (msize.x > maxwidth) {
				line.pop_back();
				line = line.substr(0, line.find_last_of(spacedelimiter));
				msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
				m_strlines.insert({ line, msize });// add line to list
				line.clear();// clear line    
				theight += msize.y + linespacing;
			}
			else {				
				strcopy = strcopy.substr(find + 1);// remove last word from the string
			};
		}
		if (!strcopy.empty())
		{
			line = line + strcopy;
			msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
			m_strlines.insert({ line , msize });
			theight += msize.y + linespacing;
		}
		auto find = m_strlines.begin();

		if (find == m_strlines.end())
			m_strlines.insert({ "" , {0,0} });

		int al = m_align, va = m_valign;
		
		float sy = rect.y + m_margin;
		if (va == 1) {
			sy = rect.y + ((rect.height - (theight + m_margin)) * 0.5f);
		}
		else if (va == 2) {
			sy = rect.y + (rect.height - (theight + m_margin));
		}

		for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
		{
			float sx = rect.x + m_margin;
			//  Align
			msize = MeasureTextEx(*m_sharedfont, iter->first.c_str(), (float)fsize, fspace);
			if (al == 1) {
				sx += (rect.width - (msize.x + m_margin)) * 0.5f;
			}
			else if (al == 2) {
				sx += (rect.width - (msize.x + m_margin));
			}
			iter->second = { sx, sy };
			sy += msize.y + linespacing;
		}
#pragma endregion
		


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
