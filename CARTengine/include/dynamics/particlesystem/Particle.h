#pragma once
#include <raylib.h>
#include "Core.h"
#include "Types.h"
#include "Clock.h"
#include "Object.h"
#include "Actor.h"
namespace cart
{

	class Sprite;
	class Particle : public Actor
	{
	public:
		Particle(World* _owningworld, const std::string& _id);
		Particle(World* _owningworld, const std::string& _id, const Particle_Properties _prop);
		void Init()override;
		void Update(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Destroy() override;
		bool IsGoneOutOfWorld();
		void SetProperties(const Particle_Properties& _props);
		Delegate<std::string> onLifeOver;
		~Particle();
	protected:
		void UpdateLocation();
	private:
		void Move(double _t);
		void Fade(double _t);
		void Scale(double _t);
		void Rotate(float _deltatime);
		Particle_Properties m_properties;
		float m_startTime;
		float m_elapsedTime;
		float m_scale;
		Color m_color;
		float m_rotation;
		float m_gravity;
		Vector2 m_velocity;
		Vector2 m_location;
		Vector2 m_calculatedLocation;	
		float m_radius;
		shared<Texture2D> m_texture;

	};

}