#pragma once
#include <string>
#include <functional>
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
			virtual void Destroy() override;
			virtual void Init() override;
			virtual void Start() override;
			virtual void Update(float _deltaTime) override;
			virtual void Draw(float deltaTime) override;
			virtual void AssetsLoadCompleted()override;
			virtual void Alert(const std::string& msg, Vector2 size, std::function<void(AlertResult)> callback);
			void CleanCycle();
			bool IsPopupActive();
			~HUD();

			template<typename ActorType, typename... Args>
			weak<ActorType> SpawnActor(Args... args);		
			
			template<typename ActorType, typename... Args>
			weak<ActorType> SpawnPopup(Args... args);

		protected:
			virtual void RestartButtonClicked(weak<Object> obj, Vector2 pos);
			virtual void QuitButtonClicked(weak<Object> obj, Vector2 pos);
			virtual void OnPopuClose(weak<Object> obj, Vector2 pos);
			Dictionary<std::string, std::function<void(AlertResult)>> m_popupCallbacks;

		private:
			shared<ProgressView> m_progressview;
			const float m_popup_width = 300.f;
			const float m_popup_height = 300.f;

			const float m_popup_min_width = 100.f;
			const float m_popup_min_height = 100.f;
			const float m_popup_max_width = 500.f;
			const float m_popup_max_height = 500.f;
			const float m_popup_margin = 10.f;
			AlertTheme m_alertTheme;

			shared<UIButton> m_blockui;
			List<shared<Actor>> m_Popups;
			List<shared<Actor>> m_Actors;
			void CloseAllPopups(weak<Object> obj, Vector2 pos);
		};

	template<typename ActorType, typename...Args>
	weak<ActorType> HUD::SpawnActor(Args...args)
	{
		shared<ActorType> newActor{ new ActorType(m_owningworld, args...) };
		m_Actors.push_back(newActor);
		return newActor;
	}

	template<typename ActorType, typename...Args>
	weak<ActorType> HUD::SpawnPopup(Args...args)
	{
		shared<ActorType> newActor{ new ActorType(m_owningworld, args...) };
		m_Popups.push_back(newActor);
		return newActor;
	}
	
}