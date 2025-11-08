#include "HUD.h"
#include "Application.h"
#include <string>
#include "World.h"
#include "Object.h"
#include "UIButton.h"
#include "Logger.h"
#include "ProgressView.h"
namespace cart
{
	
	HUD::HUD(World* _owningworld, const std::string& _id)
		:UIElement{ _owningworld, _id}	
		
	{

	}
	
	HUD::~HUD()
	{
		
	}

	void HUD::Init()
	{		
		
		m_isReady = true;
		UIElement::Init();
	}

	void HUD::Start() {
		int w = GetScreenWidth();
		int h = GetScreenHeight();
		int barwidth = 300;
		int barheight = 20;
		ProgressView pview = { m_owningworld, std::string{ "progressview" } };		
		m_progressview = std::make_shared<ProgressView>(pview);
		ProgressVeiw_Properties pprop = {};
		pprop.size = { (float)w, (float)h };
		pprop.location = { 0, 0 };
		pprop.barColor = GRAY;
		pprop.barFill = GREEN;
		pprop.color = { 255, 255, 255, 255 };
		pprop.barRect = { (float)w / 2.f - (float)barwidth / 2.f, (float)h / 2.f - (float)barheight / 2.f, (float)barwidth, (float)barheight };
		m_progressview.get()->SetUIProperties(pprop);
		m_progressview.get()->Init();
		m_progressview.get()->SetVisible(false);
		AddChild(m_progressview);
	}

	bool HUD::IsMouseOverUI(Vector2 _locmouse)
	{
		
		if (Logger::Get()->IsMouseOverUI()) {
			return true;
		}

		for(auto ui : m_children) {
			if (CheckCollisionPointRec(_locmouse, ui->GetBounds()) && ui->IsVisible() == true) return true;
		}
		return false;
	}

	void HUD::Update(float _deltaTime)
	{
		if (!m_visible)return;
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{			
			if (iter->get()->IsVisible()) {
				iter->get()->Update(_deltaTime);
			}
		}
	}

	void HUD::Draw(float _deltaTime)
	{
		if (!m_visible)return;
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			if (iter->get()->IsVisible()) {
				iter->get()->Draw(_deltaTime);
			}
		}
	}

	void HUD::ShowProgress(float v, std::string m)
	{
		if (v >= 0 && v < 1.f) {
			m_progressview.get()->SetVisible(true);
			m_progressview.get()->ShowProgress(v, m);
		}else{
			m_progressview.get()->SetVisible(false);
		}
	}
	void HUD::AssetsLoadCompleted()
	{
		Logger::Get()->Push(std::format(" {} Actor AssetsLoadCompleted()\n", GetID()));
		// Overridden this method to stop Actor from calling "Start" method
	}
	void HUD::RestartButtonClicked(weak<Object> obj, Vector2 pos)
	{

	}

	void HUD::QuitButtonClicked(weak<Object> obj, Vector2 pos)
	{
		Application* app = m_owningworld->GetApplication();
		app->QuitApplication();
	}
	
}