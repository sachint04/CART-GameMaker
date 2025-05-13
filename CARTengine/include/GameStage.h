#pragma once
#include "Object.h"
#include "Delegate.h"
namespace cart
{
    class World;
    class GameStage :public Object
    {
        public:
            GameStage(World * world);
            const World * GetWorld() const{return m_world;};
            virtual ~GameStage();
            World * GetWorld() {return m_world;};
            virtual void StartStage();
            virtual void Init();
            virtual void Update(float deltaTime);
            virtual void Draw(float deltaTime);
            void FinishStage();
            bool IsStageFinished()const {return m_stageFinished;};


            Delegate<>onStageFinished;

        protected:
        World* m_world;
        bool m_stageFinished;
        virtual void StageFinished();

    };

} // namespace cart
