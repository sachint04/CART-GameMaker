#include "dynamics/particlesystem/ParticleSystem.h"
#include "MathUtility.h"
#include "Clock.h"
#include "World.h"
#include "dynamics/particlesystem/Particle.h"
#include "MathUtility.h"
#include "Logger.h"
namespace cart {

#pragma region Constructor Init Start



	ParticleSystem::ParticleSystem( World* _owningworld, const std::string& _id, const Particle_System_Propterties& _prop) :
		Actor{ _owningworld, _id }, m_properties{ _prop }, m_spawnInterval{ 0.1f }, m_bSpawn{ false }, m_spawnStartTime(0), m_active{ false }, m_particlecount(0), m_started{false}
	{

	}


	ParticleSystem::ParticleSystem( World* _owningworld, const std::string& _id ) :
		Actor{ _owningworld ,  _id }, m_properties{ }, m_spawnInterval{ 0.01f }, m_bSpawn{ false }, m_spawnStartTime(0), m_active{ false }, m_particlecount(0), m_started{false}
	{

	}

	void ParticleSystem::Start()
	{
		if (m_started == true)return;
		m_bSpawn = true;
		m_active = true;
		m_spawnStartTime = Clock::Get().ElapsedTime();

		if (m_properties.playonstart) {

			int burstcount = (m_properties.burstonstart = true) ? m_properties.burstcount : 1;
		
			for (size_t i = 0; i < burstcount; i++)
			{
				SpawnParticles();
			}
		}		

	}

#pragma endregion

#pragma region Loop
	void ParticleSystem::Update(float _deltaTime)
	{
		if (m_active == false)return;
		if (m_bSpawn == true) {
			int mapsize = m_particleMap.size();
			if (mapsize < m_properties.maxParticles) {

				double curtime = Clock::Get().ElapsedTime();
				double elapsedtime = curtime - m_spawnStartTime;
				if (elapsedtime >= m_spawnInterval) {
					m_spawnStartTime = Clock::Get().ElapsedTime();
					SpawnParticles();
				}

			}
			else {
				//	LOG("%i PARTICLE CREATED!! out of %i ", mapsize, m_properties.maxParticles);
				m_bSpawn = false;
			}
		}

		for (size_t i = 0; i < m_particleMap.size(); i++)
		{
			if (!m_particleMap[i].expired())
				m_particleMap[i].lock()->Update(_deltaTime);
		}
	}
	void ParticleSystem::Draw(float _deltaTime)
	{
		if (m_active == false)return;

		for (size_t i = 0; i < m_particleMap.size(); i++)
		{
			if (!m_particleMap[i].expired())
				m_particleMap[i].lock()->Draw(_deltaTime);
		}
	}
#pragma endregion

#pragma region Helpers
	void ParticleSystem::SetProperties( const Particle_System_Propterties& _props)
	{
		m_properties = _props;
	}
	void ParticleSystem::SpawnParticles()
	{
		//LOG("LOOP CREATE PARTICLE SpawnParticles ");
		++m_particlecount;
		std::string id = GetId()+ "_ptcl_" + std::to_string(m_particlecount);
		Particle_Properties props = {};
		props.id = id;
		props.emittershape = m_properties.shape;
		props.emitterlocation = m_location;
		props.size = RandomVector(m_properties.minsize, m_properties.maxsize, true);
		props.lifetime = RandomRange(m_properties.minlifetime, m_properties.maxlifetime);
		props.startscale = m_properties.startscale;
		props.endscale = m_properties.endscale;
		props.startcolor = m_properties.startcolor;
		props.endcolor = m_properties.endcolor;
		props.startrotation = RandomRange(m_properties.minrotation, m_properties.maxrotation);
		props.endrotation = RandomRange(m_properties.minrotation, m_properties.maxrotation);
		props.angluarvelocity = m_properties.angularvelocity;
		props.texturepath = GetRandomTexture();
		props.texturerect = GetRandomTextureRect(props.size);
		Vector2 ranOffset = RandomPointinRect(m_properties.locationoffset);
		props.startlocation = { m_location.x + ranOffset.x, m_location.y + ranOffset.y };
		props.easing  = m_properties.easing;
		props.colorovertime = m_properties.colorovertime;
		props.burstcount = m_properties.burstcount;
		props.burstonstart = m_properties.burstonstart;
		props.speed = (m_properties.randomizeSpeed)? RandomRange(m_properties.minspeed, m_properties.maxspeed) : m_properties.speed;
		props.acceleration = m_properties.acceleration;
		props.gravity = m_properties.gravity;
		props.scaleovertime = m_properties.scaleovertime;
		

		Vector2 pc = m_location;
		if (m_properties.shape == P_PLANE2D) 
		{
			pc = GetPointOnRectangle();
			//props.startlocation = { m_location.x + pc.x * m_properties.emittersize, m_location.y + pc.y * m_properties.emittersize };
		}else if (m_properties.shape == P_CIRCLE)
		{	
			pc = GetPointInCircle();
			//props.startlocation = { m_location.x + pc.x * m_properties.emittersize, m_location.y + pc.y * m_properties.emittersize };
		}
		else if (m_properties.shape == P_CIRCLE_OUTLINE)
		{
			pc = GetPointOnCircle();
		}

			props.startlocation = { props.emitterlocation.x + (pc.x * (m_properties.emittersize * 0.5f)), 	props.emitterlocation.y + (pc.y * (m_properties.emittersize * 0.5f)) };
			//Logger::Get()->Trace(std::format("ParticleSystem::SpawnParticles() startlocation {} | {} ->  {} | {}", props.emitterlocation.x, props.emitterlocation.y , props.startlocation.x, props.startlocation.y));
			props.velocity = GetVelocityByType(m_properties.shape, props.emitterlocation, props.startlocation, props.speed);

		if (!m_properties.colorspoolstart.empty() ) {
			int rancol = RandomRange(0, m_properties.colorspoolstart.size());
			props.startcolor = m_properties.colorspoolstart[rancol];
			props.endcolor = m_properties.colorspoolend[rancol];
		}
		else {
			props.color = m_properties.color;
		}
		
		weak<Particle> p = m_owningworld->SpawnActor<Particle>(id);
		p.lock()->onLifeOver.BindAction(GetWeakRef(), &ParticleSystem::ParticleDestroyHandler);
		m_particleMap.push_back(p);

		p.lock()->SetProperties(props);
		p.lock()->SetVisible(true);
		p.lock()->Init();
	}
	void ParticleSystem::SetActive(bool _flag)
	{
		Actor::SetActive(_flag);
		for (size_t i = 0; i < m_particleMap.size(); i++)
		{	
			m_particleMap[i].lock()->SetActive(_flag);
		}
		if (_flag == false) {
			m_bSpawn = false;
			m_started = false;
		}
	}
	void ParticleSystem::RandomVelocity(float minSpeed, float maxSpeed)
	{
		Vector2 ranvec = RandomUnitVector();
		float range = RandomRange(minSpeed, maxSpeed);
		//m_properties.particleprop.velocity = { ranvec.x * range, ranvec.y * range };
	}
	Vector2 ParticleSystem::RandomVector(Vector2 min, Vector2 max, bool _inproportion)
	{
		Vector2 size = {};
		float x1 = (min.x < max.x) ? min.x : max.x;
		float x2 = (min.x > max.x) ? min.x : max.x;
		float y1 = (min.y < max.y) ? min.y : max.y;
		float y2 = (min.y > max.y) ? min.y : max.y;
		float x = RandomRange(x1, x2);
		float y = RandomRange(y1, y2);
		if (_inproportion == false) {
			size = {x, y };
		}
		else {
			float yp = y2 * (x / x2);
			size = { x, yp };
		}
		
		return size;
	}
	Vector2 ParticleSystem::RandomPointinRect(Rectangle rect)
	{
		Vector2 point = { RandomRange(rect.x, rect.width), RandomRange(rect.y, rect.height) };
		return point;
	}
    void ParticleSystem::SetLocation(Vector2 _location){
        Actor::SetLocation(_location);
    }
	std::string ParticleSystem::GetRandomTexture() const
	{		
		int ran = RandomRange(0, m_properties.texturepath.size());
		std::string path = m_properties.texturepath[ran];
		return path;
	}
	Rectangle ParticleSystem::GetRandomTextureRect(Vector2 cursize)
	{
		int ran = RandomRange(0, m_properties.texturecells.size());
		if (m_properties.istexturesprite) {
			return m_properties.texturecells[ran];
		}
		return{ 0,0, cursize.x,  cursize.y };
		
	}
	Vector2 ParticleSystem::GetVelocityByType(PARTILCE_EMITTER_SHAPE shape, Vector2 emitterloc, Vector2 plocation,  float speed)
	{
		Vector2 vel{ 0,0 };
		if (shape == PARTILCE_EMITTER_SHAPE::P_CIRCLE ||
			shape == PARTILCE_EMITTER_SHAPE::P_CIRCLE_OUTLINE ||
			shape == PARTILCE_EMITTER_SHAPE::P_CONE ||
			shape == PARTILCE_EMITTER_SHAPE::P_PLANE2D)
		{
			
			vel = NormailzeVec(Direction(emitterloc, plocation));
			vel.x *= speed;
			vel.y *= speed;

		}
		else if (shape == PARTILCE_EMITTER_SHAPE::P_LINE)
		{
			vel = { speed,  0 };
		}
		else {

		}
		return vel;
	}
#pragma endregion

#pragma region Event Handlers
	void ParticleSystem::ParticleDestroyHandler(const std::string _id)
	{	
		auto found = find_if(m_particleMap.begin(), m_particleMap.end(), [&_id](weak<Particle> p) {
			if (auto lock = p.lock()) {
				return  p.lock()->GetId() == _id;
			}
			return false;
		
		});
		if (found != m_particleMap.end()) {
			found->lock()->Destroy();
			m_particleMap.erase(found);			
		}

		if(m_properties.looping == true && m_active == true){		
			m_spawnStartTime = Clock::Get().ElapsedTime();
			m_bSpawn = true;
		}

		auto it = std::find_if(m_particleMap.begin(), m_particleMap.end(), [](weak<Particle>& ps)
		{
			if (auto lock = ps.lock()) 
			{
				return lock->IsActive();
			}
			return false;
		});
		if (it == m_particleMap.end() || m_particleMap.empty()) {
			onLifeOver.Broadcast(m_id);
		}
	}
#pragma endregion
	
#pragma region Clean up

	void ParticleSystem::Destroy()
	{
		if (IsPendingDestroy())return;
		for (auto iter  = m_particleMap.begin(); iter != m_particleMap.end();) 
		{
			iter->lock()->Destroy();
			iter = m_particleMap.erase(iter);
		}

		onLifeOver.Destroy();
		Actor::Destroy();
	}

	ParticleSystem::~ParticleSystem()
	{


	}

#pragma endregion

}
