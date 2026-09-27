#pragma once
#include <string>
#include "Core.h"
#include "dynamics/particlesystem/Particle.h"

namespace cart {
	class Actor;
	class ParticleManager {
	public:
		static ParticleManager& Get();
		void ParticleCompleteHandler(std::string _anim, bool _destroy);
		void AddParticleComp(std::string& _id, Particle* _anim);
		Particle* GetParticle(std::string& _id);
		void RemoveParticle(std::string& _id);
		~ParticleManager();
		void Update();
	protected:
		ParticleManager();
	private:
		static unique<ParticleManager> PARTICLE_CONTROLLER;
		Dictionary<std::string, Particle*> m_particleMap;
		float m_cleanCycleIter;
		float m_cleanCyclestartTime;
	};
}