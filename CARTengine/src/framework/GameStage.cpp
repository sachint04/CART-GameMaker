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
    void GameStage::Destroy()
    {

    }
     GameStage::~GameStage()
    {
    }



    void GameStage::Init()
    {
         m_isReady = true;
    } 
    void GameStage::Start()
    {
        //     LOG("Game Stage Start!");
    }
    void GameStage::Update(float deltaTime)
    {
   //     LOG("Game Stage update()");

    }
    void GameStage::Draw(float deltaTime)
    {
    }


    /*void GameStage::FinishStage()
    {
    }*/
    void GameStage::StageFinished()
    {
        m_stageFinished = true;
      //  StageFinished();      
       
    }
}