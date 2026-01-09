#pragma once
#include "World.h"
namespace cart {

	class Levelone : public World{

    public:
        Levelone(Application* owningApp);
        void Init()override;
        void Update(float _deltaTime) override;
        void Draw(float _deltaTime) override;
	};
}