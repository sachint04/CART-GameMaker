#include "tweenray/tweenMove.h"
#include "MathUtility.h"
#include "Actor.h"
#include "Clock.h"

namespace cart {


	void TweenMove::Begin() {
		/*m_readyToTween = true;
		Update();*/
	}
	
	TweenMove::TweenMove(const std::string& id, weak<Actor> actor)
		:TweenBuilderBase{ id, actor }, m_targetVec{}, m_duration{}, m_startVec{}, m_startVec3{}, m_targetVec3{}, m_easing {
		EaseInOutSine
	}, m_useFrom{ false }
	{
	//	std::cout << this << "  -> TweenMove created \n";
	};

	TweenMove::~TweenMove()
	{
		m_actor.reset();
		//std::cout << this << " -> Move Deleted\n";
	}

	void TweenMove::Update(double deltaTime) {
	
		//std::cout << "T -  " << Clock::Get().ElapsedTime() << " - TweenMove Updating  \n";
		
		if (m_go == true) {
			double delta = Clock::Get().ElapsedTime()  - m_timer;
			double t_raw = delta / m_duration;
			auto easingfunc = getEasingFunction(m_easing);
			double t = easingfunc(t_raw);
			if (t_raw <= 1.0) {
				switch (m_vectype)
				{
				case cart::Vec2D:
					Vector2 vec = LERP(m_startVec, m_targetVec, (float)t);
					m_actor.lock()->SetLocation(vec);
					break;
				case cart::Vec3D:
					Vector3 vec3 = LERP(m_startVec3, m_targetVec3, (float)t);
					m_actor.lock()->SetLocation(vec3);
					break;
				case cart::Vec4D:
					break;
				default:
					break;
				}
				//	std::string __id = m_actor.lock()->GetID();
			
				//std::cout <<  __id <<" -> Actor Moved percent  -> "<< t << std::endl;
				

				
			}
			else {
				//std::cout <<this << " -> Tween Move Complete \n";
				m_ready = false;
				m_go = false;
				switch (m_vectype)
				{
				case cart::Vec2D:
					m_actor.lock()->SetLocation(m_targetVec);
					break;
				case cart::Vec3D:
					m_actor.lock()->SetLocation(m_targetVec3);
					break;
				case cart::Vec4D:
					break;
				default:
					break;
				}

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

	
#pragma region  Move 2D

	TweenMove* TweenMove::from(Vector2 _vec)
	{
		m_vectype = VEC_TYPE::Vec2D;
		m_startVec = _vec;
		m_useFrom = true;
		//std::cout << " tweenMove : to " << _vec.x << _vec.y << std::endl;
		return this;
	}


	TweenMove* TweenMove::to(Vector2 _vec, double _duration, easing_functions _easing)
	{
		m_vectype = VEC_TYPE::Vec2D;
		if (m_useFrom == false) {
			m_startVec = m_actor.lock()->GetLocation();
		}

		m_targetVec = _vec;
		m_duration = _duration;	
		m_ready = true;	
		m_easing = _easing;
		//std::cout << this << " ->  tweenMove : to " << _vec.x << _vec.y << std::endl;
		return this;
	}
#pragma endregion

#pragma region Move3D
	TweenMove* TweenMove::from(Vector3 _vec)
	{
		
		m_startVec3 = _vec;
		m_useFrom = true;
		m_vectype = VEC_TYPE::Vec3D;
		//std::cout << " tweenMove : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

	TweenMove* TweenMove::to(Vector3 _vec, double _duration, easing_functions _easing)
	{
		m_vectype = VEC_TYPE::Vec3D;
		if (m_useFrom == false) {
			m_startVec3 = m_actor.lock()->GetLocation3();
		}

		m_targetVec3 = _vec;
		m_duration = _duration;
		m_ready = true;
		m_easing = _easing;
		//std::cout << this << " ->  tweenMove : to " << _vec.x << _vec.y << std::endl;
		return this;
	}

#pragma endregion
	TweenMove* TweenMove::onComplete(std::function<void(weak<Actor>)>&& _callback)
	{
		//EventHolder NewEvent;
		callback = _callback;
		return this;
	}
}

