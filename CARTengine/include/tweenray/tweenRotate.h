#pragma once
#include "tweenBuilderBase.h"
#include <functional>
#include "raylib.h"
#include "easing.h"
namespace cart {


	class Tween;
	class Actor;
	class TweenRotate : public TweenBuilderBase {
		typedef TweenRotate Self;
	public:

		TweenRotate(const std::string& id, weak<Actor> actor);
		~TweenRotate() override;

		Self* angle(float angle, Vector3 axis, double duration, easing_functions _easing = EaseInOutQuad);
		Self* from(float angle, Vector3 axis);
		TweenBuilderBase* onComplete(std::function<void(weak<Actor>) >&& _callback);
		void Update(double deltaTime) override;
		
		void Begin();
	protected:
		Vector3 m_axis;
		float m_startAngle;
		float m_targetAngle;
		double m_duration;
		bool m_useFrom;
		easing_functions m_easing;
		std::function<void(weak<Actor>) > callback;


	};
}
