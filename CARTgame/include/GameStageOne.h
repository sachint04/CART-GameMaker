#pragma once
#include "GameStage.h"
#include "Core.h"
namespace cart {
	class World;
	class View;
	class GameStageOne : public GameStage
	{
	public:
		GameStageOne(World* owningworld, const std::string& _id);
		~GameStageOne();
		virtual void Init() override;
		virtual void Start() override;
		void Destroy()override;

	private:
		weak<View> m_cntrl;
	};

}