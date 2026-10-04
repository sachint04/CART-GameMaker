#include "View.h"
#include <raylib.h>
#include "Types.h"
#include "Application.h"
#include "World.h"
#include "Sprite2D.h"
#include "Text.h"

namespace cart{
#pragma region Constructor & Init
	View::View(World* owningworld, const std::string& _id)
		:UIElement{owningworld, _id}
	{
	}


	void View::Init()
	{
		std::string resourcepath = m_owningworld->GetApplication()->GetAssetsPath();
		m_preloadlist.push_back({ std::string{ resourcepath + "cartengine.png"}, std::string{ resourcepath + "cartengine.png"}, ASSET_IMAGE, LOCKED });
		
		UIElement::Init();
	}

	void View::Start()
	{
		weak<Sprite2D> m_sprite = m_owningworld->SpawnActor<Sprite2D>(std::string{ "sprite" }).lock();
		AddChild(m_sprite);

		weak<Text> m_txt = m_owningworld->SpawnActor<Text>(std::string{ "welcometxt" }).lock();
		AddChild(m_txt);

#pragma region Sprite
		Rectangle rect = { 0,0, 400, 400 };
		std::string resourcepath = m_owningworld->GetApplication()->GetAssetsPath();
		std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();

		UI_Properties prop = {};
		prop.size = { 342, 100 };
		prop.location = { 0,0 };
		prop.pivot = { 0.5f, 0.5f };
		prop.anchor = { 0.5f, 0.5f ,0.5f, 0.5f };
		prop.texture = resourcepath + "cartengine.png";		
		prop.component = Layout_Component_Type::LAYOUT;
		prop.color = WHITE;
		prop.borderwidth = 2;
		prop.bordercol = PURPLE;
		if(auto lock = m_sprite.lock()){
			lock->SetUIProperties(prop);
			lock->MaintainAspectRatio(true);
			lock->SetVisible(true);
			lock->Init();
		}
#pragma endregion

#pragma region Text
		Text_Properties tprop = {};
		tprop.location = { 0,0 };
		tprop.size = { rect.width, 40 };
		tprop.location = { 0,0 };
		tprop.pivot = { 0.5f, 0.5f };
		tprop.anchor = { 0.5f, 0 ,0.5f, 0 };
		tprop.font = staticassetpath + "fonts/verdana.ttf";
		tprop.text = "Welcome to CART Engine!";
		tprop.fontsize = 40;
		tprop.minfontsize = 16;
		tprop.fontspacing = 1.4f;
		tprop.minfontspacing = 1.f;
		tprop.textcolor = BLACK;
		tprop.component = Layout_Component_Type::LAYOUT;
		tprop.align = CENTER;
		tprop.color = { 255,255, 0, 255 };
		if(auto lock = m_txt.lock()){
			lock->SetTextProperties(tprop);		
			lock->SetVisible(true);
			lock->Init();
		}

		
#pragma endregion

		UIElement::Start();
	}
#pragma endregion


#pragma region Cleanup


	void View::Destroy()
	{
		UIElement::Destroy();
	}

	View::~View()
	{
	}
#pragma endregion

}