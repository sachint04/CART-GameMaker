#include "component/ColorPalette.h"
#include "World.h"


namespace cart {
#pragma region  Constructor / Init

	ColorPalette::ColorPalette(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl, float palatteiconsize)
		:UIButton{ _owningworld, _id, isExcludedFromParentAutoControl },
		m_palatteiconsize{ palatteiconsize },
		colorState{},
		colorsRecs{},
		mousePoint{ 0,0 }		
	{
	}
	void ColorPalette::Init()
	{
		UIButton::Init();

		for (int i = 0; i < colCount ; i++)
		{
			Rectangle vec = { m_location.x + m_palatteiconsize * (i % cols) , m_location.y + (m_palatteiconsize * (i / cols)), m_palatteiconsize,m_palatteiconsize };
			colorsRecs.push_back(vec);
			colorState.push_back(0);
		}
	
	}
	void ColorPalette::Start()
	{
		UIButton::Start();
	}
#pragma endregion

#pragma region  Loop
	void ColorPalette::Update(float _deltaTime)
	{
		if (!m_visible)return;
		UIButton::Update(_deltaTime);
		
		
	}
	void ColorPalette::Draw(float _deltaTime)
	{
		if (!m_visible)return;

		UIButton::Draw(_deltaTime);

		for (int i = 0; i < colCount; i++)    // Draw all rectangles
		{
			DrawRectangleRec(colorsRecs.at(i), Fade(colors[i], colorState.at(i) ? 0.6f : 1.0f));			
		}
	}
	
#pragma endregion

#pragma region  Helper
	void ColorPalette::SetVisible(bool _flag)
	{
		UIButton::SetVisible(_flag);

	}

	void ColorPalette::EvaluateUI() {
		colorsRecs.clear();
		for (int i = 0; i < 21; i++)
		{
			Rectangle vec = { m_location.x + m_palatteiconsize * (i % 7) , m_location.y + (m_palatteiconsize * (i / 7)), m_palatteiconsize,m_palatteiconsize };
			colorsRecs.push_back(vec);			
		}
	}
#pragma endregion

#pragma region Event Handlers
	void ColorPalette::ButtonUp(Vector2 pos)
	{
		for (int i = 0; i < colorsRecs.size(); i++)
		{
			if (CheckCollisionPointRec(pos, colorsRecs[i])) {
				onPicked.Broadcast(colors[i]);
				break;
			}			
		}
	}
#pragma endregion

#pragma region  CLeanUp
	ColorPalette::~ColorPalette()
	{
		colorsRecs.clear();
		colorState.clear();
		onPicked.Destroy();
	}
#pragma endregion
}