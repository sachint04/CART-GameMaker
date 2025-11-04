#include <memory>
#include "tweenray/tweenBuilder.h"
#include "Actor.h"
#include "Core.h"
#include "tweenray/tweenMove.h"
#include "tweenray/tweenRotate.h"
#include "tweenray/tweenScale.h"
#include "tweenray/tweenColor.h"
#include "tweenray/tween.h"
using namespace cart;

	TweenBuilder::TweenBuilder(const std::string& id, weak<Actor> actor) :TweenBuilderBase{id, actor }
	{
		//std::cout << this << " - > TweenBuilder created with \n";
		Destroy();
	}

	TweenBuilder::~TweenBuilder() 
	{
	//	std::cout << this << " -> Builder deleted\n";
	}

	
	TweenMove* TweenBuilderBase::move() const{
		int index = (int)Tween::GetTweenCount();
		std::string act_id = "move_" + std::to_string(index);
		TweenMove* t = new TweenMove{ act_id, m_actor };
		Tween::AddTween(index, t);
		return t;
	}

	TweenRotate* TweenBuilderBase::rotate() const {
		int index = (int)Tween::GetTweenCount();
		std::string act_id = "rotate_" + std::to_string(index);
		TweenRotate* t = new TweenRotate{ act_id, m_actor };
		Tween::AddTween(index, t);
		return t;
	}

	TweenScale* TweenBuilderBase::scale() const {
		int index = (int)Tween::GetTweenCount();
		std::string act_id = "scale_" + std::to_string(index);
		TweenScale* t = new TweenScale{ act_id, m_actor };
		Tween::AddTween(index, t);
		return t;
	}

	TweenColor* TweenBuilderBase::color() const
	{
		int index = (int)Tween::GetTweenCount();
		std::string act_id = "color_" + std::to_string(index);
		TweenColor* t = new TweenColor{ act_id, m_actor };
		Tween::AddTween(index, t);
		return t;
	}




