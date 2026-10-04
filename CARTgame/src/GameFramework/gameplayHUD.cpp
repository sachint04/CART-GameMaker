#include "gameplayHUD.h"
#include "Application.h"
#include "World.h"
#include "UIButton.h"
namespace cart {

	GameplayHUD::GameplayHUD(World* _owningworld, const std::string& _id):HUD{_owningworld, _id}
	{
	}

	void GameplayHUD::Update(float _deltaTime)
	{
		HUD::Update(_deltaTime);
	}

	void GameplayHUD::Draw(float _deltaTime)
	{
		HUD::Draw(_deltaTime);
	}

	void GameplayHUD::Init()
	{
		// Preload assets here
		HUD::Init();
	}

	void GameplayHUD::Start()
	{

		// Creat your custom HUD elememts here
		HUD::Start();
	}


	//void GameplayHUD::RestartButtonClicked(weak<Object> obj, Vector2 pos)
	//{
	//	//HUD::RestartButtonClicked(obj,pos);
	//}

	//void GameplayHUD::QuitButtonClicked(weak<Object> obj, Vector2 pos)
	//{
	//	HUD::QuitButtonClicked(obj, pos);
	//}



}
