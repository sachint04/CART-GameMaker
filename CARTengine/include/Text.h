#pragma once
#include "UIElement.h"


namespace cart {
	class World;
	class Text : public UIElement {
	public:
		Text( World* _owningworld, const std::string& _id);
		~Text();
		void Init() override;
		void Update(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Destroy() override;
	
		virtual void SetTextProperties(Text_Properties _props);
		virtual void SetFontName(const std::string& strfnt);
		virtual std::string GetFontName();
		virtual void SetFontSize(float size);
		virtual void SetTextColor(Color col);
		virtual Color GetTextColor() { return m_textColor; };
		virtual void UpdateText(const std::string& str);
	protected:
		std::string m_text;
		std::string m_font;
		float m_fontsize;
		float m_minfontsize;
		ALIGN m_align;
		Vector2 m_textsize;
		Vector2 m_textLocation;
		float m_fontspacing;
		float m_minfontspacing;
		int m_margin;
		Color m_background;
		Color m_textColor;
		void UpdateTextLocation();
		
		
		shared<Font> m_sharedfont;


	};
}