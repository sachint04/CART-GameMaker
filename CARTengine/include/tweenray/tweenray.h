#pragma once
#include <vector>
#include <string>
#include "raylib.h"
#include "Core.h"
#include "Actor.h"
#include "Shape.h"
namespace cart {
   class World;

class tweenray {
public:
	tweenray(int actorcount);
	void Init();
	~tweenray();
  
    void Update(double deltaTime);
    void Draw(double deltaTime);

   // weak<Actor> SpwanActor(const std::string& id, int width, int height, Color col, SHAPE_TYPE shapetype) const;
private:
    float m_spawnCycleIter;
    double m_spawnCycleStartTime;
    shared<World> m_CurrentWorld;
	int m_actorcount;
	Dictionary<std::string, weak<Actor>> actormap;
    int m_count;
   
    };

  
}