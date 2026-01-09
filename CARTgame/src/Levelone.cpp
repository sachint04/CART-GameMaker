#include "Levelone.h"
#include "GameStageOne.h"
namespace cart {

	Levelone::Levelone(Application* owningApp)
		:World{ owningApp }
	{

	}
	void Levelone::Init()
	{
		AddStage(shared<GameStageOne>{new GameStageOne{ this, std::string{"stageone"} }});
		World::Init();
	}
	void Levelone::Update(float _deltaTime)
	{
		World::Update(_deltaTime);
	}
	void Levelone::Draw(float _deltaTime)
	{
		World::Draw(_deltaTime);
	}
}
