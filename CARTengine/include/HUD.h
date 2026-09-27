#pragma once
#include <string>
#include <functional>
#include "UIElement.h"
#include  "Core.h"
#include "Types.h"
#include "Logger.h"
#include "Delegate.h"
namespace cart
{

	typedef struct {
		std::string btn;
		std::function<void(std::string)> callback;
		
	}Alert_Btn_Callback;


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
			virtual void Alert(const std::string& msg, Vector2 size, std::vector<Alert_Btn_Callback> btnlist);
			virtual weak<UIButton>CreateModalContainer();
			virtual bool DestroyModal();
			void CleanCycle();
			bool IsPopupActive();
			~HUD();

			template<typename ActorType, typename... Args>
			weak<ActorType> SpawnActor(Args... args);		
			
			template<typename ActorType, typename... Args>
			weak<ActorType> SpawnPopup(Args... args);
	
			Delegate<> onModalClose;
		protected:

			virtual void QuitButtonClicked(weak<Object> obj, Vector2 pos);
			virtual void OnPopuClose(weak<Object> obj, Vector2 pos);
			virtual void OnModalClose(weak<Object> obj, Vector2 pos);
			virtual void OnModalReady(const std::string& id);

			virtual void OnAlertBtn(weak<Object> btn, Vector2 pos);
			Dictionary<std::string, std::function<void(Popup_Button_Type)>> m_popupCallbacks;
			Dictionary<std::string, std::function<void(std::string)>> m_alert_callbacks;

		private:
			shared<ProgressView> m_progressview;
			const float m_popup_width = 300.f;
			const float m_popup_height = 300.f;

			const float m_popup_min_width = 100.f;
			const float m_popup_min_height = 280.f;
			const float m_popup_max_width = 780.f;
			const float m_popup_max_height = 500.f;
			const float m_popup_margin = 10.f;
			AlertTheme m_alertTheme;
			List<shared<Actor>> m_Popups;
			List<shared<Actor>> m_Actors;
			void CloseAllPopups(weak<Object> obj, Vector2 pos);
			shared<UIButton> m_modalContent;
			weak<UIButton> m_modalbg;
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