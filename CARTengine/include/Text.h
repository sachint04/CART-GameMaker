#pragma once
#include "UIElement.h"
#include <string>
#include <string_view>
#include <vector>
#include <functional>

namespace cart {

    struct WrappedLine {
        std::string_view text;
        Rectangle size;

		WrappedLine(std::string_view t, Rectangle s) : text(t), size(s){}
    };

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
		virtual void OnLayoutChangeHandler()override;
	
		virtual void SetTextProperties(Text_Properties _props);
		virtual void SetFontName(const std::string& strfnt);
		virtual std::string GetFontName();
		virtual void SetFontSize(float size);
		virtual void SetMinFontSize(float size);
		virtual void SetMaxFontSize(float size);
		virtual float GetMinFontSize();
		virtual float GetMaxFontSize();

		virtual void SetTextColor(Color col);
		virtual Color GetTextColor() { return m_textColor; };
		virtual void SetText(const std::string& str);
		virtual void SetAligned(ALIGN _align);
		virtual void SetVAligned(V_ALIGN _valign);
      //  static void WrapText(std::string_view text, float maxwidth, float linespacing, std::vector<WrappedLine>& output, shared<Font> & font, float fsize, float fspace);
		static void WrapText(std::string& originalText, float maxWidth, float linespacing, std::vector<WrappedLine>& outWrappedLines, shared<Font>& fontPtr, float fontSize, float spacing, float uiScale);
		
		static std::string GetElidedText(Font font, const std::string& text, float max_width, float fontSize, float spacing, int padding, Vector2& textsize);

		bool ValidateTextProperties(Text_Properties _prop);
		void UpdateTextLocation();
		
		ALIGN align() { return m_align; };
		V_ALIGN valign() { return m_valign; };
	protected:
		bool m_multiline;
		bool m_overflow;
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
		float m_linespace;
		Color m_background;
		Color m_textColor;
		std::vector<WrappedLine> m_strlines;
		shared<Font> m_sharedfont;


	};
}






