#include "GameStageOne.h"
#include "Application.h"
#include "World.h"
#include "View.h"
#include "puzzleimagecontroller.h"

namespace cart{
	GameStageOne::GameStageOne(World* owningworld, const std::string& _id)
		:GameStage{ owningworld , _id }
	{
	}
	GameStageOne::~GameStageOne()
	{
	}
	void GameStageOne::Init()
	{
		std::string resourcepath = m_owningworld->GetApplication()->GetAssetsPath();

		GameStage::Init();
	}
	void GameStageOne::Start()
	{
		m_cntrl = m_owningworld->SpawnActor<View>(std::string{ "view" });
	
		UI_Properties prop = {};
		prop.size = { 400, 400 };
		prop.anchor = { 0.5f,0.5f,0.5f,0.5f };
		prop.pivot = { 0.5f , 0.5f };
		prop.location = { 0, 0 };
		prop.borderwidth = 0;
		prop.color = {255,255,255,0};
		prop.component  = Layout_Component_Type::LAYOUT;
		m_cntrl.lock().get()->SetUIProperties(prop);
		m_cntrl.lock().get()->SetVisible(true);
		m_cntrl.lock().get()->Init();
		
		GameStage::Start();
	}

	
	void GameStageOne::Destroy()
	{
		m_cntrl.reset();
		GameStage::Destroy();
	}
}