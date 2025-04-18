#include "Text.h"
#include "AssetManager.h"
#include "Types.h"
namespace cart {

	Text::Text(World* _owningworld, const std::string& _id, Vector2 _size)
		:UIElement{ _owningworld,_id,  _size },
		m_text{ },
		m_font{ },
		m_fontsize{ },
		m_margin(5),
		m_textsize{},
		m_align{ LEFT },
		m_background{ 0,0,0,0 }
	{
	}

	void Text::Init()
	{
		shared<Font>sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
		m_textsize = MeasureTextEx(*sharedfont, m_text.c_str(), m_fontsize, 1);
		UpdateLocation();	
		m_pendingUpdate = false;
	}
	

	void Text::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
		DrawRectangle(m_location.x, m_location.y, m_size.x , m_size.y, m_background);
		shared<Font>sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);	
		DrawTextEx(*sharedfont, m_text.c_str(), m_calculatedLocation, m_fontsize * m_scale,1, m_color);
	}


	void Text::UpdateLocation()
	{
		Vector2 updatedLoction = {};
		switch (m_align)
		{
		case LEFT:
			updatedLoction = { m_location.x ,  m_location.y + (m_size.y * m_scale / 2) - (m_textsize.y * m_scale)/ 2 };
			break;
		case CENTER:
			updatedLoction = { m_location.x  + (m_size.x * m_scale /2)  - (m_textsize.x * m_scale) / 2, m_location.y + (m_size.y * m_scale /2) - (m_textsize.y * m_scale)  / 2 };
			break;

		case RIGHT:
			updatedLoction = { m_location.x + (m_size.x * m_scale) - (m_textsize.x * m_scale),  m_location.y + (m_size.y * m_scale / 2) - (m_textsize.y * m_scale)/ 2 };
			break;
		}
	
		m_calculatedLocation = updatedLoction;
	}

	void Text::SetTextProperties(Text_Properties _prop)
	{
		m_font = _prop.font;
		m_text = _prop.text;
		m_fontsize = _prop.fontsize;
		m_align = _prop.align;	
		m_color = _prop.color;	
		m_location = _prop.location;
		m_background = _prop.textbackground;
		UpdateLocation();
	}

	Text::~Text()
	{
	//	LOG("%s Text Deleted!", m_id.c_str());
	}
}
