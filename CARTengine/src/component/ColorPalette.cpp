#include "component/ColorPalette.h"
#include "World.h"


namespace cart {
#pragma region  Constructor / Init

	ColorPalette::ColorPalette(World* _owningworld, const std::string& _id, float palatteiconsize)
		:UIElement{ _owningworld, _id},
		m_palatteiconsize{ palatteiconsize },
		colorState{},
		colorsRecs{},
		mousePoint{0,0}
	{
	}
	void ColorPalette::Init()
	{
		UIElement::Init();
		for (int i = 0; i < 21; i++)
		{
			Rectangle vec = { m_location.x + m_palatteiconsize * (i % 7) , m_location.y + (m_palatteiconsize * (i / 7)), m_palatteiconsize,m_palatteiconsize };
			colorsRecs.push_back(vec);
			colorState.push_back(0);
		}
	
	}
#pragma endregion

#pragma region  Loop
	void ColorPalette::Update(float _deltaTime)
	{
		if (!m_visible)return;
		UIElement::Update(_deltaTime);
		mousePoint = GetMousePosition();
		if (IsMouseButtonReleased(0)) {
			for (int i = 0; i < 21; i++)
			{
				if (CheckCollisionPointRec(mousePoint, colorsRecs[i])) colorState.at(i) = 1;
				else colorState.at(i) = 0;
			}
		}
		
	}
	void ColorPalette::Draw(float _deltaTime)
	{
		if (!m_visible)return;

		UIElement::Draw(_deltaTime);
		Color colors[21] = {
		   DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE, DARKBROWN,
		   GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK, YELLOW,
		   GREEN, SKYBLUE, PURPLE, BEIGE };

		const char* colorNames[21] = {
			"DARKGRAY", "MAROON", "ORANGE", "DARKGREEN", "DARKBLUE", "DARKPURPLE",
			"DARKBROWN", "GRAY", "RED", "GOLD", "LIME", "BLUE", "VIOLET", "BROWN",
			"LIGHTGRAY", "PINK", "YELLOW", "GREEN", "SKYBLUE", "PURPLE", "BEIGE" };

		for (int i = 0; i < 21; i++)    // Draw all rectangles
		{
			DrawRectangleRec(colorsRecs.at(i), Fade(colors[i], colorState.at(i) ? 0.6f : 1.0f));

			if (IsKeyDown(KEY_SPACE) || colorState.at(i) == 1)
			{
				onPicked.Broadcast(colors[i]);
				colorState.at(i) = 0;
			}
		}
	}
	
#pragma endregion

#pragma region  Helper
	void ColorPalette::SetVisible(bool _flag)
	{
		UIElement::SetVisible(_flag);

	}
#pragma endregion


#pragma region  CLeanUp
	ColorPalette::~ColorPalette()
	{
		colorsRecs.clear();
		colorState.clear();
	}
#pragma endregion
}