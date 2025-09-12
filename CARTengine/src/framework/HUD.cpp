#include "HUD.h"
#include "Application.h"
#include <string>
#include "World.h"
#include "Object.h"
#include "UIButton.h"
#include "Logger.h"
namespace cart
{
	
	HUD::HUD(World* _owningworld, const std::string& _id)
		:UIElement{ _owningworld, _id},		
		m_AlreadyInit{false}
	{

	}
	HUD::~HUD()
	{
		
	}
	void HUD::NativeInit()
	{
		if (!m_AlreadyInit)
		{
			m_AlreadyInit = true;
			Init();
		}
	}

	void HUD::Init()
	{
		
		m_AlreadyInit = true;
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
		//UIElement::Update(_deltaTime);

	}

	void HUD::Draw(float _deltaTime)
	{
		//UIElement::Draw(_deltaTime);
		
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