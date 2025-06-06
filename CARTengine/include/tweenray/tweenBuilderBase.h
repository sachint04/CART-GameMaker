#pragma once
#include "Core.h"
#include "Object.h"
#include "Types.h"

namespace cart {


	class TweenMove;
	class TweenRotate;
	class TweenScale;
	class TweenColor;

	class Tween;
	class Actor;
	class TweenBuilderBase : public Object{

	protected:
		weak<Actor> m_actor;
		double m_timer;
		bool m_go;
		bool m_ready;
		VEC_TYPE m_vectype = VEC_TYPE::Vec2D;

	public:
		
		TweenBuilderBase(const std::string& id, weak<Actor> actor);
		/*
		operator Tween() const
		{
			return std::move(*m_tween);

		}
		*/

		virtual ~TweenBuilderBase();

		TweenMove* move() const;
		TweenRotate* rotate() const;
		TweenScale* scale() const;
		TweenColor* color() const;

		virtual void Update(double deltaTime) {
			
		}





	};
}