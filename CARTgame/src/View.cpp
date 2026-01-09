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
		m_preloadlist.push_back(std::string{ resourcepath + "cartengine.png" });
		UIElement::Init();
	}

	void View::Start()
	{
		weak<Sprite2D> m_sprite = m_owningworld->SpawnActor<Sprite2D>(std::string{ "sprite" });
		AddChild(m_sprite);

		weak<Text> m_txt = m_owningworld->SpawnActor<Text>(std::string{ "txt" });
		AddChild(m_txt);

#pragma region Sprite
		Rectangle rect = { 0,0, 400, 400 };
		std::string resourcepath = m_owningworld->GetApplication()->GetAssetsPath();
		std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();

		UI_Properties prop = {};
		prop.size = { rect.width, rect.height };
		prop.location = { 0,0 };
		prop.pivot = { 0.5f, 0.5f };
		prop.anchor = { 0.5f, 0.5f ,0.5f, 0.5f };
		prop.texture = resourcepath + "cartengine.png";
		
		prop.component = LAYOUT;
		prop.color = WHITE;

		m_sprite.lock().get()->SetUIProperties(prop);
		m_sprite.lock().get()->MaintainAspectRatio(true);
		m_sprite.lock().get()->SetVisible(true);
		m_sprite.lock().get()->Init();
#pragma endregion

#pragma region Text
		Text_Properties tprop = {};
		tprop.location = { 0,0 };
		tprop.size = { rect.width, 40 };
		tprop.location = { 0,380 };
		tprop.pivot = { 0.5f, 0.5f };
		tprop.anchor = { 0.5f, 0 ,0.5f, 0 };
		tprop.font = staticassetpath + "fonts/verdana.ttf";
		tprop.fontsize = 20;
		tprop.minfontsize = 16;
		tprop.fontspacing = 1.4f;
		tprop.minfontspacing = 1.f;
		tprop.textcolor = BLUE;
		tprop.component = LAYOUT;
		tprop.align = CENTER;
		tprop.color = { 255,255,255, 0 };
		tprop.text = "Welcome to CART Engine!";
		m_txt.lock().get()->SetTextProperties(tprop);
		m_txt.lock().get()->SetVisible(true);
		m_txt.lock().get()->Init();

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