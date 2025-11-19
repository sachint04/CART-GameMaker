#include "ProgressView.h"
#include <algorithm> 
#include <raylib.h>
#include "Application.h"
#include "World.h"
#include "CARTjson.h"
#include "Text.h"
namespace cart{

#pragma region Constructor & Init
	ProgressView::ProgressView(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl, PROGRESS_VIEW_TYPE _type)
		:UIElement{ _owningworld, _id, isExcludedFromParentAutoControl },
		m_pType{ _type },
		m_pValue{0},
		m_pMessge{""},
		m_barCol{GRAY},
		m_barFill{GREEN},
		m_barRect{},
		m_txtCol{BLACK}
	{
		
	}

	void ProgressView::Init()
	{
		UIElement::Init();

		
	}
	void ProgressView::Start()
	{
	
	}
#pragma endregion

#pragma region Game Loop
	void ProgressView::Update(float deltaTime)
	{
		if (m_visible)
		{
			UIElement::Update(deltaTime);

		}

	}
	void ProgressView::Draw(float deltaTime)
	{
		if (m_visible)
		{
			UIElement::Draw(deltaTime);
			if (m_pType == P_BAR)
			{
				DrawBar();

			}
			else if (m_pType == P_DISC)
			{
				DrawDisc();
			}
			else if (m_pType == P_CLOCK)
			{
				DrawClock();
			}
			else {
				Logger::Get()->Trace("Error! ProgressView::ShowProgress() View Type not defined \0");
			}
		}
	}
#pragma endregion

#pragma region Helpers
	void ProgressView::ShowProgress(float v, const std::string& m)
	{
		//Logger::Get()->Trace(std::format("ProgressView::ShowProgress() {} | {} \0", v, m),LOG_WARNING);
		m_pValue = v;
		m_pMessge = m;
	}
	

	void ProgressView::DrawBar()
	{
		float displayval = (m_pValue < 0.1f) ? 0.1f : m_pValue;
		// Draw Bar background
		DrawRectangle(m_barRect.x, m_barRect.y, m_barRect.width, m_barRect.height, m_barCol);
		// Draw bar fill
		//DrawRectangle(m_barRect.x + 1, m_barRect.y + 1, (m_barRect.width * displayval) -2, m_barRect.height - 2, m_barFill);// solid color bar
		DrawRectangleGradientH(m_barRect.x + 1, m_barRect.y + 1, (m_barRect.width * displayval) - 2, m_barRect.height - 2, { 1, 181,248,255 }, { 20,255,102,255 });// Gradient fill bar
		DrawText(m_pMessge.c_str(), m_barRect.x, m_barRect.y - 15.f, 10, DARKGREEN);
		
	}
	void ProgressView::DrawDisc()
	{
	}
	void ProgressView::DrawClock()
	{
	}
	void ProgressView::SetUIProperties(ProgressVeiw_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
		m_barCol = _prop.barColor;
		m_barFill = _prop.barFill;
		m_barRect = _prop.barRect;
		m_txtCol = _prop.msgCol;

	}

#pragma endregion

#pragma region CleanUp

	void ProgressView::Destroy()
	{		
		UIElement::Destroy();

	}

	ProgressView::~ProgressView()
	{
	}
#pragma endregion
}