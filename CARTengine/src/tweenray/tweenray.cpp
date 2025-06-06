

#include <iostream>
#include "tweenray/tweenray.h"
#include "tweenray/tween.h"
#include "Shape.h"
#include "World.h"
#include "MathUtility.h"


#include "Clock.h"
#include "easing.h"

namespace cart {

	tweenray::tweenray(int actorcount) :m_actorcount{ actorcount }, m_CurrentWorld{ nullptr }, m_count{ 0 }, m_spawnCycleIter{ 2.f }, m_spawnCycleStartTime{0} {
		
	}

	void tweenray::Init()
	{
	

	}

	tweenray::~tweenray()
	{
	}

	void tweenray::Update(double deltaTime) {
		Tween::Update(deltaTime);

	}

	void tweenray::Draw(double deltaTime) {
		
	}
/*
	weak<Actor> tweenray::SpwanActor(const std::string& id, int width, int height, Color col, SHAPE_TYPE shapetype)const
	{
		weak<Shape> actor = m_CurrentWorld->SpawnActor<Shape>(id, width, height, col);
		return actor;
	}

	tweenray::~tweenray() {


	}
	*/
}