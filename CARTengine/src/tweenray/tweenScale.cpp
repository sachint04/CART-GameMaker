#include "tweenray/tweenScale.h"
#include "MathUtility.h"
#include "Actor.h"
#include "Clock.h"

namespace cart {


	void TweenScale::Begin() {
		/*m_readyToTween = true;
		Update();*/
	}
	
	TweenScale::TweenScale(const std::string& id, weak<Actor> actor)
		:TweenBuilderBase{ id, actor }, m_startScale{}, m_targetScale{}, m_duration{},  m_easing{EaseInOutSine}, m_useFrom{false}
	{
	//	std::cout << this << "  -> TweenScale created \n";
	};

	TweenScale::~TweenScale()
	{
		m_actor.reset();
		//std::cout << this << " -> scale Deleted\n";
	}

	void TweenScale::Update(double deltaTime) {
		if (m_actor.expired()) {
			Destroy();
			return;
		}
		if (m_go == true) {
			double delta = Clock::Get().ElapsedTime()  - m_timer;
			double t_raw = delta / m_duration;
			auto easingfunc = getEasingFunction(m_easing);
			double t = easingfunc(t_raw);
			if (t_raw <= 1.0) {
				m_actor.lock().get()->SetTweening(true);
				float scale = LERP(m_startScale, m_targetScale, (float)t);
			//	std::string __id = m_actor.lock()->GetID();
			
				//std::cout <<  __id <<" -> Actor Moved percent  -> "<< t << std::endl;
				
				m_actor.lock()->SetScale(scale);

				
			}
			else {
				//std::cout <<this << " -> Tween Move Complete \n";
				m_ready = false;
				m_go = false;
				m_actor.lock().get()->SetTweening(false);
				m_actor.lock()->SetScale(m_targetScale);

				if(callback)
					callback(m_actor);
				
				Destroy();

			}
			return;
		}


		if (m_ready == true) {
	
			m_timer = Clock::Get().ElapsedTime();
			m_go = true;
		}
	}

	

	TweenScale* TweenScale::from(float scale)
	{

		m_startScale = scale;
		m_useFrom = true;
		//std::cout << " TweenScale : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	TweenScale* TweenScale::to(float scale, double _duration, easing_functions _easing)
	{
		if (m_useFrom == false)
			m_startScale = m_actor.lock()->GetScale();


		m_targetScale= scale;
		m_duration = _duration;	
		m_ready = true;	
		m_easing = _easing;
		//std::cout << this << " ->  TweenScale : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	TweenScale* TweenScale::onComplete(std::function<void(weak<Actor>)>&& _callback)
	{
		//EventHolder NewEvent;
		callback = _callback;
		return this;
	}
}

