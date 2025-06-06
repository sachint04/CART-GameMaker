#pragma once
#include "tweenBuilderBase.h"
#include <functional>
#include "raylib.h"
#include "easing.h"
namespace cart {


	class Tween;
	class Actor;
	class TweenScale : public TweenBuilderBase {
		typedef TweenScale Self;
	public:

		TweenScale(const std::string& id, weak<Actor> actor);
		~TweenScale() override;

		Self* to(float scale, double _duration, easing_functions _easing = EaseInOutQuad);
		Self* from(float scale);
		Self* onComplete(std::function<void(weak<Actor>) >&& _callback);
		void Update(double deltaTime) override;
		
		void Begin();
	protected:
		float m_startScale;
		float m_targetScale;
		double m_duration;
		easing_functions m_easing;
		bool m_useFrom;
		std::function<void(weak<Actor>) > callback;
	};
}
