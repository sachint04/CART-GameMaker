#pragma once
#include "Object.h"
#include "Delegate.h"
namespace cart
{
    class World;
    class GameStage :public Object
    {
        public:
            GameStage(World * owningworld, const std::string& _id);
            const World * GetWorld() const{return m_owningworld;};
            virtual ~GameStage();
            World * GetWorld() {return m_owningworld;};
            virtual void StartStage();
            virtual void Init();
            virtual void Update(float deltaTime);
            virtual void Draw(float deltaTime);
 //           void FinishStage();
            bool IsStageFinished()const {return m_stageFinished;};


            Delegate<>onStageFinished;

        protected:
        World* m_owningworld;
        bool m_stageFinished;
        virtual void StageFinished();

    };

} // namespace cart
