#include "HUD.h"
#include <string>
#include "Application.h"
#include "AssetManager.h"
#include "World.h"
#include "Object.h"
#include "UIButton.h"
#include "SimpleButton.h"
#include "Logger.h"
#include "ProgressView.h"
#include "CARTjson.h"
#include "Text.h"
#include "UICanvas.h"

namespace cart
{
	
#pragma region Constructor Init & Start
	HUD::HUD(World* _owningworld, const std::string& _id)
		:UIElement{ _owningworld, _id }, m_Popups{}
		
	{

	}
	void HUD::Init()
	{		
		
		UIElement::Init();
	}
	void HUD::Start() {

		int w = GetScreenWidth();
		int h = GetScreenHeight();
		int barwidth = 300;
		int barheight = 20;
		ProgressView pview = { m_owningworld, std::string{ "progressview" }, true };
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

		UIElement::Start();
	}
#pragma endregion

#pragma region Loop
	void HUD::Update(float _deltaTime)
	{
		if (!m_visible)return;
		UIElement::Update(_deltaTime);

		for (auto& actor : m_Actors)
		{
			if (actor && !actor->IsPendingDestroy())
			{
				actor->Update(_deltaTime);
			}
		}

		for (auto& popup : m_Popups)
		{
			if (popup && !popup->IsPendingDestroy())
			{
				auto parent = popup->GetParent().lock();
				if (!parent || !parent->IsUI())
				{
					popup->Update(_deltaTime);					
				}
			}
		}
	}

	void HUD::Draw(float _deltaTime)
	{
		if (!m_visible)return;

		for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
		{
			if (!iter->get()->IsPendingDestroy())
				iter->get()->Draw(_deltaTime);
		}


		for (auto& popup : m_Popups)
		{
			if (popup && !popup->IsPendingDestroy())
			{
				popup->Draw(_deltaTime);
			}
		}

	}
#pragma endregion

#pragma region Helper
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
	void HUD::ShowProgress(float v, std::string m)
	{
		if (!m_progressview)return;
		if (v >= 0 && v < 1.f) {
			m_progressview.get()->SetVisible(true);
			m_progressview.get()->ShowProgress(v, m);
		}else{
			m_progressview.get()->SetVisible(false);
		}
	}
	void HUD::Alert(const std::string& msg , Vector2 size, std::vector<Alert_Btn_Callback> btnlist)
	{
		auto& appdata = CARTjson::GetAppData();
		auto staticassetspath = m_owningworld->GetApplication()->GetStaticAssetsPath();
		std::string strfontid = appdata["cart"]["systemfont"];
		std::string strfont = appdata["cart"]["font"][strfontid]["path"];
		float fsize = 18.f;// appdata["cart"]["systemfontsize"];
		float fspacing = appdata["cart"]["fontspacing"];
		const float w = size.x;// std::max(std::min(size.x, m_popup_max_width), m_popup_min_width);
		const float h = size.y;// std::max(std::min(size.y, m_popup_max_height), m_popup_min_height);
		std::string msgcopy = { msg };
	

		shared<Font> font = AssetManager::Get().LoadFontAsset(staticassetspath + strfont, fsize);
		std::string strc = std::to_string(m_Popups.size());

#pragma region Object Creation
		std::string pid = { "popup_" + strc };
		weak<UIElement> alert = SpawnPopup<UIElement>(pid);		

		weak<Text> alerttxt = m_owningworld->SpawnActor<Text>(std::string{ "alertmsg" });
		alert.lock()->AddChild(alerttxt);
		
		weak<UIElement> btn_container = SpawnPopup<UIElement>(std::string{ "hud-alert-btn-container" });
		alert.lock()->AddChild(btn_container);
		
		std::vector<weak<SimpleButton>> tmpbtnlist;
		for (auto iter = btnlist.begin(); iter != btnlist.end(); ++iter) {
			weak<SimpleButton> btn = SpawnPopup<SimpleButton>(std::string{ "alertmsg_"+iter->btn});
			btn_container.lock()->AddChild(btn);
			tmpbtnlist.push_back(btn.lock());
			if (auto lock = btn.lock()) {
				lock->onButtonUp.BindAction(GetWeakRef(), &HUD::OnAlertBtn);				
			}
		}
#pragma endregion
#pragma region Alert
		if (auto lock = alert.lock()) {
			UI_Properties uiprop = {};
			uiprop.pivot = { 0.5f,0.5f };
			uiprop.anchor = { 0.5f,0.5f, 0.5f ,0.5f };
			uiprop.location = { 0, 0 };
			uiprop.size = { w, h };
			uiprop.color = m_alertTheme.background;
			uiprop.component = LAYOUT;
			uiprop.borderwidth = 1.f;
			uiprop.bordercol = GRAY;
			lock->SetUIProperties(uiprop);
			lock->SetVisible(true);
			lock->Init();
			//lock->UpdateLayout();
		}
#pragma endregion

Vector2 buttonsize = { 80, 26 };
#pragma region Button Container
		if (auto lock = btn_container.lock())
		{
			UI_Properties container_prop = {};
			container_prop.size = { size.x, buttonsize.y };
			container_prop.anchor = { 0.5f, 1, 0.5f,1 };
			container_prop.pivot = { 0.5f, 1.f };
			container_prop.location = { 0, -5.f };
			container_prop.component = H_LAYOUT;
			container_prop.layout_props = { CENTER, TOP, 0.f, 10.f };
			container_prop.color = {255,255,255,0};
			//container_prop.blockscale = true;
			lock->SetUIProperties(container_prop);
			lock->SetVisible(true);
			lock->Init();
			//lock->UpdateLayout();
		}
#pragma endregion
#pragma region Alert Message
		if (auto lock = alerttxt.lock())
		{
			Text_Properties tprop = {};
			tprop.align = CENTER;
			tprop.valign = MIDDLE;
			tprop.textcolor = m_alertTheme.textcol;
			tprop.font = staticassetspath + strfont;
			tprop.fontsize = fsize;
			tprop.minfontsize = 12;
			tprop.fontspacing = fspacing;			
			tprop.component = LAYOUT;
			tprop.anchor = { 0.5f, 0, 0.5f, 0 };
			tprop.pivot = { 0.5f, 0 };
			tprop.location = { 0, 5 };
			tprop.size = { w - 10.f, h - 45.f };
			tprop.text = msgcopy;
			tprop.multiline = true;
			tprop.linespace = 1.2f;
			tprop.borderwidth = 1.f;
			tprop.color = { 240,240,240,255 };
			lock->SetTextProperties(tprop);
			lock->SetVisible(true);
			lock->Init();
			lock->UpdateLayout();
		}
#pragma endregion
#pragma region Alert Buttons
		Btn_Text_Properties btntxtprop = {};
		btntxtprop.font = staticassetspath + strfont;
		btntxtprop.fontsize = fsize;
		btntxtprop.minfontsize = 12;
		btntxtprop.fontspacing = fspacing;
		btntxtprop.minfontspace = fspacing;
		btntxtprop.btncol = m_alertTheme.button;
		btntxtprop.overcol = m_alertTheme.button;
		btntxtprop.downcol = m_alertTheme.button;
		btntxtprop.disablecol = m_alertTheme.button;
		btntxtprop.textAlign = ALIGN::CENTER;
		btntxtprop.component = NO_LAYOUT;
		//btntxtprop.blockscale = true;
		btntxtprop.shapetype = SHAPE_TYPE::RECTANGLE;
		btntxtprop.location = { 0, 0 };
		btntxtprop.pivot = { 0, 0 };
		btntxtprop.anchor = { 0.5f,0.5f,0.5f,0.5f };
		btntxtprop.size = buttonsize;
		btntxtprop.textcolor = BLACK;
		btntxtprop.borderwidth = 1.f;
		btntxtprop.bordercol = BLACK;

		for (auto iter = btnlist.begin(); iter != btnlist.end(); ++iter) 
		{
			std::string Id = "alertmsg_" + iter->btn;
			auto find = std::find_if(tmpbtnlist.begin(), tmpbtnlist.end(), [&Id](weak<SimpleButton>& btn)
			{
				if (auto lock = btn.lock())
				{
					return lock->GetId() == Id;
				}
				return false;
			});
			if (find != tmpbtnlist.end())
			{
				if (auto lock = find->lock())
				{
					btntxtprop.text = iter->btn;
					lock->SetButtonProperties(btntxtprop);
					lock->SetVisible(true);
					lock->Init();
					lock->UpdateLayout();				
					m_alert_callbacks.insert({ Id, iter->callback });

					json data = { {"popupId", alert.lock()->GetId()}, {"label", iter->btn} };
					lock->SetCustomData(data);
				}
			}	
		}
		alerttxt.lock()->UpdateLayout();
#pragma endregion
/*
		weak<UIButton> okbtn = SpawnPopup<UIButton> (std::string{ pid + "_OK" });		
		okbtn.lock()->onButtonUp.BindAction(GetWeakRef(), &HUD::OnPopuClose);
		alert.lock()->AddChild(okbtn);
		
		weak<UIButton> cancelbtn = SpawnPopup<UIButton>(std::string{ pid+"_CANCLE" });
		cancelbtn.lock()->onButtonUp.BindAction(GetWeakRef(), &HUD::OnPopuClose);

		btntxtprop.text = "Ok";
		btntxtprop.location = {-60, -5};

		if (auto lock = okbtn.lock()) {
			lock->SetButtonProperties(btntxtprop);
			lock->SetVisible(true);
			lock->Init();
		}
	
		btntxtprop.location = { 60, -5 };
		btntxtprop.text = "Cancle";
		if (auto lock = cancelbtn.lock())
		{
			lock->SetButtonProperties(btntxtprop);
			lock->SetVisible(true);
			lock->Init();
		}
		cancelbtn.lock()->UpdateLayout();
		okbtn.lock()->UpdateLayout();
		alerttxt.lock()->UpdateLayout();
		*/
	}	
	void HUD::QuitButtonClicked(weak<Object> obj, Vector2 pos)
	{
		Application* app = m_owningworld->GetApplication();
		app->QuitApplication();
	}
	void HUD::CloseAllPopups(weak<Object> obj, Vector2 pos)
	{
		for (auto iter = m_Popups.begin(); iter != m_Popups.end(); )
		{
			iter->get()->Destroy();
			iter = m_Popups.erase(iter);
		}
	}
	
	bool HUD::IsPopupActive()
	{
		for (auto iter = m_Popups.begin(); iter != m_Popups.end(); ++iter)
		{
			if (iter->get()->IsVisible()) {
				return true;
			}
		}
		return false;
	}
	weak<UIButton> HUD::CreateModalContainer()
	{

		if (!m_modalContent) {
			Btn_Properties prop = {};
			prop.component = LAYOUT;
			prop.pivot = { 0,0 };
			prop.anchor = { 0,0,1,1 };
			prop.size = { 0,0 };
			prop.location = { 0,0 };
			prop.color = { 0,0,0,100 };
			prop.btncol = m_alertTheme.blocker;
			prop.overcol = m_alertTheme.blocker;
			prop.downcol = m_alertTheme.blocker;
			prop.disablecol = m_alertTheme.blocker;
			prop.cursorstyle = 0;

			m_modalContent = SpawnPopup<UIButton>(std::string{ "modalcontent" }).lock();
			
			m_modalContent->onReady.BindAction(GetWeakRef(), &HUD::OnModalReady);// listen to child ready event
			m_modalContent->onButtonUp.BindAction(GetWeakRef(), &HUD::OnModalClose);
			m_modalContent->SetButtonProperties(prop);
			m_modalContent->SetVisible(true);
			m_modalContent->Init();	
		}

		return m_modalContent;
	}
	bool HUD::DestroyModal() {
		if (m_modalContent) {
			OnModalClose(m_modalContent->GetWeakRef(), { 0,0 });
			return true;
		}
		return false;
	}

#pragma endregion

#pragma region Event Handlers
	void HUD::OnPopuClose(weak<Object> obj, Vector2 pos)
	{
		weak<UIButton> btn = std::static_pointer_cast<UIButton>(obj.lock());
		std::string id = obj.lock().get()->GetId();
		auto find = id.find(std::string("_OK"));
		std::string popid = id.substr(0, id.find_last_of("_"));
		if (find != std::string::npos) {
			auto callback = m_popupCallbacks.find(popid);
			callback->second(Popup_Button_Type::Alert_Ok);
			m_popupCallbacks.erase(callback);
		}
		for (size_t i = 0; i < m_Popups.size(); i++)
		{
			if (m_Popups[i].get()->GetId().compare(popid) == 0) {
				m_Popups[i].get()->Destroy();
				break;
			}
		}
	}
	void HUD::OnModalClose(weak<Object> obj, Vector2 pos)
	{
		
		if (m_modalContent) {
			onModalClose.Broadcast();
			m_modalContent->Destroy();
			m_modalContent.reset();
		}
	}
	void HUD::OnModalReady(const std::string& id)
	{
		if (m_modalContent) {
			//.World::UI_CANVAS.get()->UpdateLayout();
		}
	}
	void HUD::OnAlertBtn(weak<Object> btn, Vector2 pos)
	{
		auto find = m_alert_callbacks.find(btn.lock()->GetId());
		if (find != m_alert_callbacks.end())
		{
			weak<SimpleButton> elem = std::dynamic_pointer_cast<SimpleButton>(btn.lock());
			json btndata = elem.lock()->GetCustomData();


			if (auto lock = elem.lock())
			{
				if (find->second)
				{
					if (btndata.contains("label")) {
						find->second(btndata["label"]);
					}
				}			
			}

			if (btndata.contains("popupId")) {
				std::string popuId = btndata["popupId"];
				auto find = std::find_if(m_Popups.begin(), m_Popups.end(), [popuId](shared<Actor>& p) {
					return p->GetId() == popuId;
				});
				if (find != m_Popups.end()) {
					find->get()->Destroy();
				}
			}
		}
		m_alert_callbacks.clear();		
	}
	void HUD::AssetsLoadCompleted()
	{
		Logger::Get()->Trace(std::format(" {} Actor AssetsLoadCompleted()\n", GetId()));
		// Overridden this method to stop Actor from calling "Start" method
	}
#pragma endregion

#pragma region CleanUp
	void HUD::Destroy()
	{
		if (m_isPendingDestroy) {

			for (auto iter = m_Popups.begin(); iter != m_Popups.end();)
			{
				iter->get()->Destroy();
				iter = m_Popups.erase(iter);
			}

			for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
			{
				iter->get()->Destroy();
				iter = m_Actors.erase(iter);
			}
			for (auto iter = m_popupCallbacks.begin(); iter != m_popupCallbacks.end();)
			{
				iter = m_popupCallbacks.erase(iter);
			}
			
			onModalClose.Destroy();

			UIElement::Destroy();
		}

	}
	void HUD::CleanCycle() {

		std::erase_if(m_Actors, [this](const auto& actor) {

			bool shouldErase =  actor.use_count() <= 1 || actor->IsPendingDestroy();
			if (shouldErase && actor) {
				actor->Destroy();
			}
			return shouldErase;
		});


		std::erase_if(m_Popups, [this](const auto& popup) {

			bool shouldErase = popup.use_count() <= 1 || popup->IsPendingDestroy();
			if (shouldErase && popup) {
				popup->Destroy();
			}
			return shouldErase;
		});

	}
	HUD::~HUD()
	{

	}
#pragma endregion
}