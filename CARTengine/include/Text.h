#pragma once
#include <raylib.h>
#include "UIElement.h"


namespace cart {
	class World;
	class Text : public UIElement {
	public:
		Text( World* _owningworld, const std::string& _id, Vector2 _size);
		~Text();
		void SetTextProperties(Text_Properties _props);
		void Init() override;
		void Draw(float _deltaTime) override;
		void Destroy() override;
		virtual void SetFontName(const std::string& strfnt);
		virtual void SetFontSize(float size);
		virtual void SetTextColor(Color col);
	protected:
		std::string m_text;
		std::string m_font;
		float m_fontsize;
		ALIGN m_align;
		Vector2 m_textsize;
		float m_fontspacing;
		int m_margin;
		Color m_background;
		Color m_textColor;
		void UpdateLocation() override;
		
		
		shared<Font> m_sharedfont;


	};
}