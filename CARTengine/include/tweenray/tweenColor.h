#pragma once
#include "tweenBuilderBase.h"
#include <functional>
#include "raylib.h"
#include "easing.h"
namespace cart {


	class Tween;
	class Actor;
	class TweenColor : public TweenBuilderBase {
		typedef TweenColor Self;
	public:

		TweenColor(const std::string& id, weak<Actor> actor);
		~TweenColor() override;

		Self* to(Color col, double duration, easing_functions _easing = EaseInOutQuad);
		Self* from(Color col);
		TweenBuilderBase* onComplete(std::function<void(weak<Actor>) >&& _callback);
		void Update(double deltaTime) override;
		
		void Begin();
	protected:
		Color m_startCol;
		Color m_targetCol;
		double m_duration;
		//EventHolder evt;
		easing_functions m_easing;
		std::function<void(weak<Actor>) > callback;
		bool m_useFrom;

	};
}
