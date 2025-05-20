#pragma once
#include <raylib.h>
#include "UIElement.h"


namespace cart {

	class Text : public UIElement {

	public:
		Text( World* _owningworld, const std::string& _id, Vector2 _size);
		~Text();
		void SetTextProperties(Text_Properties _props);
		void Init() override;
		void Draw(float _deltaTime) override;
		void Destroy() override;

	protected:
		std::string m_text;
		std::string m_font;
		int m_fontsize;
		ALIGN m_align;
		Vector2 m_textsize;
		float m_fontspacing;
		int m_margin;
		Color m_background;
		void UpdateLocation() override;


	};
}