#include "HUD.h"
#include <string>
#include "Application.h"
#include "AssetManager.h"
#include "World.h"
#include "Object.h"
#include "UIButton.h"
#include "Logger.h"
#include "ProgressView.h"
#include "CARTjson.h"
#include "Text.h"
namespace cart
{
	
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

		// blocker button for popups		
		Btn_Properties btnprop = {};
		btnprop.component = LAYOUT;
		btnprop.pivot = {0,0};
		btnprop.anchor = {0,0,1,1};
		btnprop.size = { 0,0 };
		btnprop.location = { 0,0 };
		btnprop.color = { 0,0,0,0 };
		btnprop.btncol = m_alertTheme.blocker;
		btnprop.overcol = m_alertTheme.blocker;
		btnprop.downcol = m_alertTheme.blocker;
		btnprop.disablecol = m_alertTheme.blocker;



		m_blockui = std::make_shared<UIButton>( m_owningworld, std::string("blockpopupbtn") );
		m_blockui.get()->onButtonUp.BindAction(GetWeakRef(), &HUD::CloseAllPopups);
		m_blockui.get()->SetButtonProperties(btnprop);
		m_blockui.get()->SetVisible(false);
		m_blockui.get()->Init();


		UIElement::Start();
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
		UIElement::Update(_deltaTime);
		
		for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
		{
			if (!iter->get()->IsPendingDestroy())
			iter->get()->Update(_deltaTime);
		}
		if (IsPopupActive()) {
			m_blockui.get()->SetVisible(true);
			m_blockui.get()->Update(_deltaTime);
		}
		else {
			m_blockui.get()->SetVisible(false);
		}
		
		for (size_t i = 0; i < m_Popups.size(); i++)
		{
			if (!m_Popups[i].get()->IsPendingDestroy())
				m_Popups[i].get()->Update(_deltaTime);

		}
	}

	void HUD::Draw(float _deltaTime)
	{
		if (!m_visible)return;
		UIElement::Draw(_deltaTime);
		for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
		{
			if (!iter->get()->IsPendingDestroy())
			iter->get()->Draw(_deltaTime);
		}

			
		m_blockui.get()->Draw(_deltaTime);
		

		for (size_t i = 0; i < m_Popups.size(); i++)
		{
			if (!m_Popups[i].get()->IsPendingDestroy())
				m_Popups[i].get()->Draw(_deltaTime);

		}
		
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

	void HUD::AssetsLoadCompleted()
	{
		Logger::Get()->Trace(std::format(" {} Actor AssetsLoadCompleted()\n", GetId()));
		// Overridden this method to stop Actor from calling "Start" method
	}
	void HUD::Alert(const std::string& msg , Vector2 size, std::function<void(AlertResult)> callback)
	{
		auto& appdata = CARTjson::GetAppData();
		auto& staticassetspath = m_owningworld->GetApplication()->GetStaticAssetsPath();
		std::string strfontid = appdata["cart"]["systemfont"];
		std::string strfont = appdata["cart"]["font"][strfontid]["path"];
		float fsize = appdata["cart"]["systemfontsize"];
		float fspacing = appdata["cart"]["fontspacing"];
		const float w = std::max(std::min(size.x, m_popup_max_width), m_popup_min_width);
		const float h = std::max(std::min(size.y, m_popup_max_height), m_popup_min_height);
		std::string msgcopy = { msg };

		Text_Properties tprop = {};
		tprop.align = LEFT;
		tprop.textcolor = BLACK;
		tprop.font = staticassetspath + strfont;
		tprop.fontsize = fsize;
		tprop.fontspacing = fspacing;
		tprop.component = LAYOUT;
		tprop.anchor = { 0, 0, 1, 1 };
		tprop.pivot = { 0, 0};
		tprop.location = { 10, 10};
		tprop.size = { 10, 10};
		tprop.text = msgcopy;
		tprop.multiline = true;
		tprop.color = { 255,255,255,0 };
		

		UI_Properties uiprop = {};
		uiprop.pivot = { 0.5f,0.5f };
		uiprop.anchor = { 0.5f,0.5f, 0.5f ,0.5f };
		uiprop.location = { 0, 0 };
		uiprop.size = { w, h };
		uiprop.color = m_alertTheme.background;
		uiprop.component = LAYOUT;

		Btn_Text_Properties btntxtprop = {};
		btntxtprop.font = staticassetspath + strfont;
		btntxtprop.fontsize = fsize;
		btntxtprop.fontspacing = fspacing;
		btntxtprop.btncol = m_alertTheme.button;
		btntxtprop.overcol = m_alertTheme.hover;
		btntxtprop.downcol = m_alertTheme.down;
		btntxtprop.disablecol = m_alertTheme.disable;
		btntxtprop.textAlign = ALIGN::CENTER;

		btntxtprop.shapetype = SHAPE_TYPE::RECTANGLE;
		btntxtprop.component = LAYOUT;
		btntxtprop.anchor = {0.5f, 1, 0.5f, 1};
		btntxtprop.pivot = {0.5f, 1};
		btntxtprop.size = {60, 30};
		



		shared<Font> font = AssetManager::Get().LoadFontAsset(staticassetspath + strfont, fsize);
		std::string strc = std::to_string(m_Popups.size());
		std::string pid = { "popup_" + strc };


		weak<UIElement> alert = SpawnPopup<UIElement>(pid);
		
		m_popupCallbacks.insert({ alert.lock().get()->GetId(), callback });

		alert.lock()->SetUIProperties(uiprop);
		alert.lock()->SetVisible(true);
		alert.lock()->Init();
		

		weak<Text> alerttxt = SpawnPopup<Text>(std::string{ "alertmsg" });
		alerttxt.lock()->SetTextProperties(tprop);
		alerttxt.lock()->SetVisible(true);
		alerttxt.lock()->Init();

		btntxtprop.text = "Ok";
		btntxtprop.location = {-60, -10};
		weak<UIButton> okbtn = SpawnPopup<UIButton> (std::string{ pid + "_OK" });
		okbtn.lock()->onButtonUp.BindAction(GetWeakRef(), &HUD::OnPopuClose);
		okbtn.lock()->SetButtonProperties(btntxtprop);
		okbtn.lock()->SetVisible(true);
		okbtn.lock()->Init();
		
		btntxtprop.location = { 60, -10 };
		btntxtprop.text = "Cancle";
		weak<UIButton> cancelbtn = SpawnPopup<UIButton>(std::string{ pid+"_CANCLE" });
		cancelbtn.lock()->onButtonUp.BindAction(GetWeakRef(), &HUD::OnPopuClose);
		cancelbtn.lock()->SetButtonProperties(btntxtprop);
		cancelbtn.lock()->SetVisible(true);
		cancelbtn.lock()->Init();
		
		alert.lock()->AddChild(alerttxt);
		alert.lock()->AddChild(okbtn);
		alert.lock()->AddChild(cancelbtn);

		

	}
	void HUD::RestartButtonClicked(weak<Object> obj, Vector2 pos)
	{

	}

	void HUD::QuitButtonClicked(weak<Object> obj, Vector2 pos)
	{
		Application* app = m_owningworld->GetApplication();
		app->QuitApplication();
	}

	void HUD::OnPopuClose(weak<Object> obj, Vector2 pos)
	{

		weak<UIButton> btn = std::static_pointer_cast<UIButton>(obj.lock());
		std::string id = obj.lock().get()->GetId();
		auto find = id.find(std::string("_OK"));
		std::string popid = id.substr(0, id.find_last_of("_"));
		if (find != std::string::npos) {
			auto callback  = m_popupCallbacks.find(popid);
			callback->second(AlertResult::Alert_Ok);
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
	
	void HUD::CloseAllPopups(weak<Object> obj, Vector2 pos)
	{
		for (auto iter = m_Popups.begin(); iter != m_Popups.end(); )
		{
			iter->get()->Destroy();
			iter = m_Popups.erase(iter);
		}
	}
#pragma region Helper
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
			
			m_blockui.get()->Destroy();
			m_blockui.reset();

			UIElement::Destroy();
		}

	}
	void HUD::CleanCycle() {
		for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
		{
			if (iter->use_count() == 1)
			{
				iter->reset();
				iter = m_Actors.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		for (auto iter = m_Popups.begin(); iter != m_Popups.end();)
		{
			if (iter->use_count() == 1)
			{
				iter->reset();
				iter = m_Popups.erase(iter);
			}
			else
			{
				++iter;
			}
		}

	}
	HUD::~HUD()
	{

	}
#pragma endregion
}