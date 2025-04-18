#pragma once
#include "HUD.h"
namespace cart {

	class GameplayHUD : public HUD {

	public:
		GameplayHUD(World* _owningworld, const std::string& _id);
		virtual void Update(float _deltaTime) override;
		virtual void Draw(float _deltaTime) override;
		Delegate<> onRestartBtnClicked;
		Delegate<> onQuitBtnClicked;
		virtual void Init() override;

	protected:
		void RestartButtonClicked() override;
		void QuitButtonClicked() override;

	private:
	};
}
