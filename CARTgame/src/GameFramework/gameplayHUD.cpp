#include "gameplayHUD.h"

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

	#pragma region Exit Button
		UI_Properties btnui = {};
		Vector2 screenSize = m_owningworld->GetAppWindowSize();
		btnui.location = { screenSize.x - 50.f, screenSize.y - 50.f };
		btnui.size = { 50.f, 50.f };
		btnui.color = { 82, 59, 116, 255 };
		Btn_Text_Properties btnprop = {};
		btnprop.font = FONT_NAME;
		btnprop.fontsize = 12.f;
		btnprop.btncol = GRAY;
		btnprop.color = GRAY;
		btnprop.overcol = DARKGRAY;
		btnprop.text = "X";
		btnprop.textAlign = ALIGN::CENTER;
		btnprop.size = { 20.f, 20.f };
		btnprop.location = { screenSize.x - 25.f, 12.f };
		btnprop.textcolor = BLACK;
		std::string exitbtnid = "exitbtn";

		weak<UIButton> extbtn = m_owningworld->SpawnActor<UIButton>(exitbtnid, btnprop.size);
		extbtn.lock()->SetTextProperties(btnprop);
		extbtn.lock()->SetVisible(true);
		extbtn.lock()->SetActive(true);
		extbtn.lock()->Init();
		extbtn.lock()->onButtonUp.BindAction(GetWeakRef(), &GameplayHUD::QuitButtonClicked);
		btnprop = {};// clear struct
#pragma endregion

		HUD::Init();
	}


	void GameplayHUD::RestartButtonClicked(weak<Object> obj, Vector2 pos)
	{
		HUD::RestartButtonClicked(obj,pos);
	}

	void GameplayHUD::QuitButtonClicked(weak<Object> obj, Vector2 pos)
	{
		HUD::QuitButtonClicked(obj, pos);
	}



}
