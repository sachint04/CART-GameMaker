#pragma once
#include "UIElement.h"


namespace cart {
	class World;
	class Text : public UIElement {
	public:
		Text( World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);
		~Text();
		void Init() override;
		void Start() override;
		void Update(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Destroy() override;
	
		virtual void SetTextProperties(Text_Properties _props);
		virtual void SetFontName(const std::string& strfnt);
		virtual std::string GetFontName();
		virtual void SetFontSize(float size);
		virtual void SetTextColor(Color col);
		virtual Color GetTextColor() { return m_textColor; };
		virtual void SetText(const std::string& str);
		virtual void SetAligned(ALIGN _align);
		virtual void SetVAligned(V_ALIGN _valign);

		bool ValidateTextProperties(Text_Properties _prop);
		
		ALIGN align() { return m_align; };
		V_ALIGN valign() { return m_valign; };
	protected:
		bool m_multiline;
		ALIGN m_align;
		V_ALIGN m_valign;
		Vector2 m_textLocation;
		std::string m_text;
		std::string m_font;
		int m_margin;
		float m_fontsize;
		float m_minfontsize;
		float m_maxfontsize;
		float m_fontspacing;
		float m_minfontspacing;
		Color m_background;
		Color m_textColor;
		Dictionary<std::string, Vector2> m_strlines;
		void UpdateTextLocation();
		shared<Font> m_sharedfont;


	};
}