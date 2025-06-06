#include "tweenray/tweenBuilderBase.h"
#include "tweenray/tween.h"
#include "tweenray/tweenMove.h"
#include "tweenray/tweenRotate.h"
#include "tweenray/tweenScale.h"
#include "tweenray/tweenColor.h"

namespace cart {
	

	TweenBuilderBase::TweenBuilderBase(const std::string&  id, weak<Actor> actor)
		:Object{ id }, m_actor{ actor }, m_timer{}, m_go{}, m_ready{}, m_vectype{ VEC_TYPE::Vec2D }
	{
		//	std::cout << this << " -> TweenBuilderBase created\n";
	}

	TweenBuilderBase::~TweenBuilderBase()
	{
		m_actor.reset();
		//std::cout << this << " -> Base deleted \n";
	};


	

}