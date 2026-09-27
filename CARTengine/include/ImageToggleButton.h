#pragma once
#include "ImageButton.h"
#include "Core.h"

namespace cart {

	class World;
	class ImageToggleButton : public ImageButton {
	
	public:
		ImageToggleButton(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);
		void Init() override;
		void SetButtonProperties(Btn_Properties _prop)override;
		 void SetButtonProperties(Btn_Text_Properties _prop)override;
		 void ButtonUp(Vector2 pos)override;
		 void Toggle();
		 bool IsToggled() { return m_bToggled; };

		 void Destroy() override;
		 ~ImageToggleButton();
		
		
	private:
		bool m_bToggled;
		std::string m_primaryTexture;
		std::string m_secondaryTexture;
		Texture_Source_Rect m_primaryRect;
		Texture_Source_Rect m_secondaryRect;

	};

}