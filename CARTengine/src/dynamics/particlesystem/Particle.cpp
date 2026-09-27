#include <iostream>
#include <raylib.h>
#include "Application.h"
#include "dynamics/particlesystem/Particle.h"
#include "MathUtility.h"
#include "AssetManager.h"
#include "World.h"
#include "easing.h"
#include "Logger.h"
namespace cart
{
#pragma region Constructor Init Start


	Particle::Particle(World* _owningworld, const std::string& _id)
		: Actor{ _owningworld,  _id }, m_properties{ }, m_scale(1.f), m_color{}, m_rotation(0), m_velocity{}, m_location{}, m_calculatedLocation{}, m_elapsedTime(0), m_startTime(0), m_radius{ 0 }, m_gravity{0}
	{

	}

	Particle::Particle( World* _owningworld, const std::string& _id, const Particle_Properties _prop)
		: Actor{ _owningworld ,  _id }, m_properties{ _prop }, m_scale(1.f), m_color{}, m_rotation(0), m_velocity{}, m_location{}, m_calculatedLocation{}, m_elapsedTime(0), m_startTime(0), m_radius{ 0 }, m_gravity{0}
	{
		
	}

	void Particle::Init()
	{
		shared<Texture2D> texture2d = AssetManager::Get().LoadTextureAsset(m_properties.texturepath);
		m_startTime = Clock::Get().ElapsedTime();
		m_elapsedTime = 0;
		Actor::Init();
	}

#pragma endregion


#pragma region Loop

	void Particle::Update(float _deltaTime)
	{
		if (!m_active)return;
		
		double curtime = Clock::Get().ElapsedTime();
		m_elapsedTime = curtime - m_startTime;
		double t_raw = m_elapsedTime / m_properties.lifetime;
		
		if (t_raw >= 1 || IsGoneOutOfWorld()) {
			
			SetActive(false);
			if (onLifeOver.mCallbacks.size() > 0) {
				onLifeOver.Broadcast(m_properties.id);
			}			
		}
		else {
			float screenwidth = GetScreenWidth();
			float screenheight = GetScreenHeight();

			auto easingfunc = getEasingFunction(m_properties.easing);
			double t = easingfunc(t_raw);
			Fade(t_raw);
			Scale(t);
			Rotate(_deltaTime);
			Move(t);
		}
	}
	void Particle::Draw(float _deltaTime)
	{
		if (!m_active || !m_visible)return;
		if (!m_texture) {
			m_texture = AssetManager::Get().LoadTextureAsset(m_properties.texturepath);	
		}
		Rectangle source = { 0,0,(float)m_texture->width, (float)m_texture->height };
		Rectangle dest = { m_location.x,m_location.y,  (float)m_properties.size.x * m_scale, (float)m_properties.size.y * m_scale };
		Vector2 origin = { dest.width * 0.5f, dest.height * 0.5f };



#ifndef __EMSCRIPTEN__
		if (Application::GPT_TIER > 1) {
			BeginBlendMode(m_properties.blendmode);
		}
#endif // !__EMSCRIPTEN__
		
		DrawTexturePro(*m_texture, m_properties.texturerect, dest, origin, m_rotation, m_color);

#ifndef __EMSCRIPTEN__	
		if (Application::GPT_TIER > 1) {
			EndBlendMode();
		}
#endif // !__EMSCRIPTEN__
	}

#pragma endregion

#pragma region Helpers
	void Particle::SetProperties(const Particle_Properties& _props)
	{
		m_properties = _props;
		m_location = m_properties.startlocation;
	}
	void Particle::Move(double _t)
	{

		float _deltaTime = Clock::Get().DeltaTime();
		m_velocity.x = m_properties.velocity.x * (m_properties.speed * m_properties.acceleration);
		m_velocity.y = m_properties.velocity.y * (m_properties.speed * m_properties.acceleration);
		
	
		m_location.x += (m_velocity.x * _t);
		m_location.y += (m_velocity.y * _t) + (m_properties.gravity * _deltaTime);
		//Logger::Get()->Trace(std::format("void Particle::Move() id {}   location {} | {} ", GetId(), m_location.x, m_location.y));
		UpdateLocation();
	}
	void Particle::Fade(double _t)
	{
		if (m_properties.colorovertime == true) {
			m_color = LERP(m_properties.startcolor, m_properties.endcolor, _t, true);
		}
		else {
			m_color = m_properties.color;
		}
	}
	void Particle::Rotate(float _deltatime)
	{
		if (m_properties.emittershape == P_CIRCLE || m_properties.emittershape == P_PLANE2D || m_properties.emittershape == P_CIRCLE_OUTLINE) {
			m_rotation += (m_properties.angluarvelocity.y * _deltatime);
		}
	}
	void Particle::Scale(double _t)
	{
		if (m_properties.scaleovertime == true) {
			m_scale = LERP(m_properties.startscale, m_properties.endscale, _t);
		}
		else {
			m_scale = m_properties.startscale;
		}
	}
	void Particle::UpdateLocation()
	{
		
		float w = m_properties.size.x * m_scale;
		float h = m_properties.size.y * m_scale ;
		m_calculatedLocation = { m_location.x - (w * 0.5f), m_location.y - (h * 0.5f) };

	}
	bool Particle::IsGoneOutOfWorld()
	{
		float worldwidth = GetScreenWidth();
		float worldheight = GetScreenHeight();
		float screenbuffer = 50.f;
		float left = screenbuffer * -1.f;
		float top = screenbuffer * -1.f;
		float right = worldwidth + screenbuffer;
		float bottom = worldheight + screenbuffer;

		if (m_properties.gravity > 0) {
			if (m_location.x > right ||
				m_location.y > bottom ||
				(m_location.x + (m_properties.size.x * m_scale)) < left)
			{
				return true;
			}
		}
		else {
			if (m_location.x > right ||
				(m_location.y > bottom) ||
				(m_location.x + (m_properties.size.x * m_scale)) < left ||
				(m_location.y + (m_properties.size.y * m_scale)) < top)
			{
				return true;
			}
		}
		return false;
	}

	
#pragma endregion


#pragma region Clean up
	void Particle::Destroy()
	{
		if (m_isPendingDestroy)return;

		

		if (m_texture)
		{
		//	std::string txtpath = GetId() + "_tex";	
			//AssetManager::Get().UnloadTextureAsset(txtpath);
			m_texture.reset();
		}
		onLifeOver.Destroy();

		SetActive(false);

		Actor::Destroy();
	}

	Particle::~Particle()
	{

	}
#pragma endregion
}