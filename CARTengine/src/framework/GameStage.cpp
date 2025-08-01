#include "GameStage.h"
#include "World.h"
#include "Core.h"
namespace cart
{
    GameStage::GameStage(World* owningworld, const std::string& _id)
    :Object{_id},
    m_owningworld{owningworld},
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
        m_stageFinished = false;
        LOG("Initialize Game Stage");
    }
    void GameStage::Update(float deltaTime)
    {
   //     LOG("Game Stage update()");

    }
    void GameStage::Draw(float deltaTime)
    {
    }
    void* GameStage::GetOutPut()
    {
        return m_sessionData;
    }

    void GameStage::SetInutPut(void* data)
    {
        m_sessionData = data;
    }

    /*void GameStage::FinishStage()
    {
    }*/
    void GameStage::StageFinished()
    {
        m_stageFinished = true;
      //  StageFinished();      
        LOG("Stage Finished");
    }
}