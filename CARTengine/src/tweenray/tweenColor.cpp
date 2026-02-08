#include "Actor.h"
#include "tweenray/tweenColor.h"
#include "MathUtility.h"
#include "Clock.h"
namespace cart {


	void TweenColor::Begin() {
		/*m_readyToTween = true;
		Update();*/
	}
	
	TweenColor::TweenColor(const std::string& id, weak<Actor> actor)
		:TweenBuilderBase{ id, actor }, m_startCol{0}, m_targetCol{0}, m_duration {0}, m_easing{ EaseInOutSine }, m_useFrom{false}
	{
		//std::cout << this << "  -> tweeb color created \n";
	};

	TweenColor::~TweenColor()
	{
		m_actor.reset();
		//std::cout << this << " -> rotate Deleted\n";
	}

	void TweenColor::Update(double deltaTime) {
	
		//std::cout << "T -  " << Clock::Get().ElapsedTime() << " - TweenColor Updating  \n";
		
		if (m_go == true) {
			double delta = Clock::Get().ElapsedTime()  - m_timer;
			double t_raw = delta / m_duration;
			auto easingfunc = getEasingFunction(m_easing);
			double t = easingfunc(t_raw);
			if (t_raw <= 1.0) {
				m_actor.lock().get()->SetTweening(true);
				Color col = LERP(m_startCol, m_targetCol, (float)t, true);
//				std::string __id = m_actor.lock()->GetID();
				m_actor.lock()->SetColor(col);
			}
			else {
				//std::cout <<this << " -> Tween Move Complete \n";
				m_actor.lock()->SetColor(m_targetCol);
				m_actor.lock().get()->SetTweening(false);
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

	TweenColor* TweenColor::from(Color col)
	{
		m_startCol = col;
		m_useFrom = true;
		//std::cout << " tweenMove : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	TweenColor* TweenColor::to( Color col, double duration, easing_functions _easing)
	{
		if(m_useFrom == false)
			m_startCol = m_actor.lock()->GetColor();


		m_targetCol = col;
		m_duration = duration;
		m_ready = true;
		m_easing = _easing;
		//std::cout << " TweenColor : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	

	TweenBuilderBase* TweenColor::onComplete(std::function<void(weak<Actor>)>&& _callback)
	{
		//EventHolder NewEvent;
		callback = _callback;
		return this;
	}
}

