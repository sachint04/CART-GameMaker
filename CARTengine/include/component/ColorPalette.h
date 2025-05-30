#pragma once
#include "UIElement.h"
#include "Delegate.h"
namespace cart
{
	class World;
	class ColorPalette : public UIElement
	{

	public:
		ColorPalette(World* _owningworld, const std::string& _id, float palatteiconsize);
		void Init()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void SetVisible(bool _flag)override;
		~ColorPalette();

		Delegate<Color> onPicked;
	private:
		float m_palatteiconsize;
		std::vector<int>colorState;
		std::vector<Rectangle> colorsRecs;
		Vector2 mousePoint = { 0.0f, 0.0f };

	};
}