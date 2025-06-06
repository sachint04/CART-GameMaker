#include <memory>
#include "tweenray/tween.h"
#include "Core.h"
#include "Actor.h"
#include "tweenray/tweenBuilder.h"

#include "Clock.h"
namespace cart {


	Dictionary<int, TweenBuilderBase*> Tween::tweenlist = {};
	List<TweenBuilderBase*> Tween::pendingDeleteTween = {};
	float Tween::m_cleanCycleIter =  0.3f;
	double Tween::m_cleanCycleStartTime =  0;

	Tween::Tween() {

	}

	TweenBuilder* Tween::create(weak<Actor> actor)
	{
		int id = (int)tweenlist.size() + 1;
		std::string tw_id = "builder_" + std::to_string(id);
		TweenBuilder* t = new TweenBuilder{ tw_id, actor};
//		shared<TweenBuilder> t = std::make_shared<TweenBuilder>(TweenBuilder{ tw_id, actor });
		tweenlist.insert({ id, t});			
		return t;
	}

	void Tween::Update(double deltaTime) {
		//std::cout << deltaTime  << "  - tween update \n";
		//if (tweenlist.size() == 0)return;

		double curTime = Clock::Get().ElapsedTime();
		double elapsetime = curTime - m_cleanCycleStartTime;
		long garbagesize = GetSizeOfPendingTweens();
		if (garbagesize >= (double)5000 || elapsetime >= m_cleanCycleIter) {
			CleanCycle();
			m_cleanCycleStartTime = Clock::Get().ElapsedTime();
		}



		for (auto iter = tweenlist.begin(); iter != tweenlist.end();) {
			if (iter->second->IsPendingDestroy() == true) {
				pendingDeleteTween.push_back(iter->second);				
				iter = tweenlist.erase(iter);				
			}
			else {
				iter->second->Update(deltaTime);
				++iter;
			}
		}
	}


	rsize_t Tween::GetTweenCount()
	{
		return tweenlist.size();
	}

	void Tween::AddTween(int id, TweenBuilderBase* tween)
	{
	/*	auto found = tweenlist.find(id);
		if (found != tweenlist.end())
			return;*/

		tweenlist.insert({ id, tween });


	}


	void Tween::CleanCycle() {

		for (auto iter = pendingDeleteTween.begin(); iter != pendingDeleteTween.end();)
		{
				//LOG("WORLD CleanClcyle %i ", iter->);
			//iter->reset();
			delete *iter;
		//	*iter = nullptr;
			iter = pendingDeleteTween.erase(iter);
		}
		pendingDeleteTween.clear();
	}

	long Tween::GetSizeOfPendingTweens() {

		long memsize = sizeof(TweenBuilderBase);
		long arrsize = sizeof(pendingDeleteTween);
		long countarry = pendingDeleteTween.size();
		long totalmem = memsize * countarry;
		return totalmem;

	}
}