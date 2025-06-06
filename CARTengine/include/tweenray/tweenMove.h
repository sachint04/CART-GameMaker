#pragma once
#include "tweenBuilderBase.h"
#include <functional>
#include "raylib.h"
#include "easing.h"
namespace cart {


	class Tween;
	class Actor;
	class TweenMove : public TweenBuilderBase {
		typedef TweenMove Self;
	public:

		TweenMove(const std::string& id, weak<Actor> actor);
		~TweenMove() override;

		Self* to(Vector2 _vec, double _duration, easing_functions _easing = EaseInOutQuad);
		Self* from(Vector2 _vec);	
		
		Self* to(Vector3 _vec, double _duration, easing_functions _easing = EaseInOutQuad);
		Self* from(Vector3 _vec);

		Self* onComplete(std::function<void(weak<Actor>) >&& _callback);
		void Update(double deltaTime) override;
		
		void Begin();
	protected:
		Vector2 m_startVec;
		Vector2 m_targetVec;
		
		Vector3 m_startVec3;
		Vector3 m_targetVec3;

		double m_duration;
		easing_functions m_easing;
		bool m_useFrom;
		//EventHolder evt;
		std::function<void(weak<Actor>) > callback;
	};
}
