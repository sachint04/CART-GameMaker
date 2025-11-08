#pragma once
#include <string>
#include "UIElement.h"
#include  "Core.h"
namespace cart
{
	class World;
	class Object;
	class UIButton;
	class ProgressView;
	class HUD: public UIElement
	{
		public: 
			HUD(World* _owningworld, const std::string& _id);

			bool IsMouseOverUI(Vector2 _locmouse);
			void ShowProgress(float v, std::string m);
			virtual void Init() override;
			virtual void Start() override;
			virtual void Update(float _deltaTime) override;
			virtual void Draw(float deltaTime) override;
			virtual void AssetsLoadCompleted()override;
			~HUD();

		protected:
			virtual void RestartButtonClicked(weak<Object> obj, Vector2 pos);
			virtual void QuitButtonClicked(weak<Object> obj, Vector2 pos);
		private:
			shared<ProgressView> m_progressview;


		};

	
}