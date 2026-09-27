#pragma once
#include "Delegate.h"
#include "Core.h"
#include "Types.h"
#include "Actor.h"

namespace cart {
	class Particle;
	class ParticleSystem : public Actor{
	public:
		ParticleSystem( World* _owningworld, const std::string& _id );
		ParticleSystem(World* _owningworld, const std::string& _id, const Particle_System_Propterties& _prop);
		void Start()override;
		void Update(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void SetActive(bool _flag) override;
		void SetLocation(Vector2 _location) override;
		void SetProperties(const Particle_System_Propterties& _props);
		void Destroy()override;

        Delegate<std::string> onLifeOver;
		~ParticleSystem();
	private:
		void RandomVelocity(float minSpeed, float maxSpeed);
		Vector2 RandomVector(Vector2 min, Vector2 max, bool _inproportion);
		Vector2 RandomPointinRect(Rectangle rect);
		void SpawnParticles();
		std::string GetRandomTexture()const;
		Rectangle GetRandomTextureRect(Vector2 cursize);
		Vector2 GetVelocityByType(PARTILCE_EMITTER_SHAPE shape, Vector2 emitterloc, Vector2 plocation, float speed);
		void ParticleDestroyHandler(const std::string _id);		

		std::vector<weak<Particle>> m_particleMap;
		Particle_System_Propterties m_properties;
		double m_spawnInterval;
		bool m_bSpawn;
		bool m_started;
		double m_spawnStartTime;
		bool m_active;
		int m_particlecount;
	};


}