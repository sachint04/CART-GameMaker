#include "tweenray/tweenRotate.h"
#include "MathUtility.h"
#include "Actor.h"
#include "Clock.h"
namespace cart {


	void TweenRotate::Begin() {
		/*m_readyToTween = true;
		Update();*/
	}
	
	TweenRotate::TweenRotate(const std::string& id, weak<Actor> actor)
		:TweenBuilderBase{ id, actor }, m_startAngle{0}, m_targetAngle{0}, m_duration {0}, m_easing{ EaseInOutSine }, m_useFrom{false}
	{
		//std::cout << this << "  -> TweenRotate created \n";
	};

	TweenRotate::~TweenRotate()
	{
		m_actor.reset();
		//std::cout << this << " -> rotate Deleted\n";
	}

	void TweenRotate::Update(double deltaTime) {
	
		//std::cout << "T -  " << Clock::Get().ElapsedTime() << " - TweenRotate Updating  \n";
		
		if (m_go == true) {
			double delta = Clock::Get().ElapsedTime()  - m_timer;
			double t_raw = delta / m_duration;
			auto easingfunc = getEasingFunction(m_easing);
			double t = easingfunc(t_raw);
			if (t_raw <= 1.0) {
				float vec = LERP(m_startAngle, m_targetAngle, (float)t);
				std::string __id = m_actor.lock()->GetID();
			
				m_actor.lock()->SetRotation(vec);

				
			}
			else {
				//std::cout <<this << " -> Tween Move Complete \n";
				m_ready = false;
				m_go = false;
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

	

	TweenRotate* TweenRotate::angle(float angle, double duration, easing_functions _easing)
	{
		if(m_useFrom == false)
			m_startAngle = m_actor.lock()->GetRotation();
	
		
		m_targetAngle = angle;
		m_duration = duration;
		m_ready = true;
		m_easing = _easing;
		//std::cout << " TweenRotate : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	TweenRotate* TweenRotate::from(float angle)
	{
		m_startAngle = angle;
		return this;
	}

	

	TweenBuilderBase* TweenRotate::onComplete(std::function<void(weak<Actor>)>&& _callback)
	{
		//EventHolder NewEvent;
		callback = _callback;
		return this;
	}
}

