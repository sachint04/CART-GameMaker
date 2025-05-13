#include "GameStage.h"
#include "World.h"
#include "Core.h"
namespace cart
{
    GameStage::GameStage(World* world)
    :Object{},
    m_world{world},
        m_stageFinished{false}
    {


    }
    
     GameStage::~GameStage()
    {
    }

    void GameStage::StartStage()
    {
        Init();
    }

    void GameStage::Init()
    {
        LOG("Initialize Game Stage");
    }
    void GameStage::Update(float deltaTime)
    {
   //     LOG("Game Stage update()");

    }
    void GameStage::Draw(float deltaTime)
    {
    }
    void GameStage::FinishStage()
    {
        m_stageFinished = true;
        StageFinished();
        onStageFinished.Broadcast();
    }
    void GameStage::StageFinished()
    {
        LOG("Stage Finished");
    }
}