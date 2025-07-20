#pragma once
#include <string>
#include "UIElement.h"
namespace cart
{
	class World;
	class Object;
	class UIButton;
	class HUD: public UIElement
	{
		public: 
			HUD(World* _owningworld, const std::string& _id);

			bool HasInit() { return m_AlreadyInit; }
			void NativeInit();
			bool IsMouseOverUI(Vector2 _locmouse);
			virtual void Init() override;
			virtual void Update(float _deltaTime) override;
			virtual void Draw(float deltaTime) override;
		
			~HUD();

		protected:
			virtual void RestartButtonClicked(weak<Object> obj, Vector2 pos);
			virtual void QuitButtonClicked(weak<Object> obj, Vector2 pos);
		private:
			bool m_AlreadyInit;		
		

		};

	
}