#include "dynamics/particlesystem/ParticleManager.h"
#include "Actor.h"
#include "Clock.h"
#include "Logger.h"
namespace cart {

#pragma region SINGLETON
        unique<ParticleManager> ParticleManager::PARTICLE_CONTROLLER{ nullptr };

    ParticleManager& ParticleManager::Get()
    {
        if (!PARTICLE_CONTROLLER) {
            PARTICLE_CONTROLLER = unique<ParticleManager>{ new ParticleManager };
        }
        return *PARTICLE_CONTROLLER;
    }
#pragma endregion

#pragma region  LIFE CYCLE
    ParticleManager::ParticleManager() :m_cleanCycleIter(5.f), m_cleanCyclestartTime(0)
    {
        m_cleanCyclestartTime = Clock::Get().ElapsedTime();
    }

    ParticleManager::~ParticleManager()
    {
        auto iter = m_particleMap.begin();
        if (iter != m_particleMap.end()) {
            iter = m_particleMap.erase(iter);
        }

       // LOG("ParticleContoller Deleted!");
    }
#pragma endregion

#pragma region  LOOP

    void ParticleManager::Update()
    {
        for (auto iter = m_particleMap.begin(); iter != m_particleMap.end();)
        {
            if (iter->second->IsPendingDestroy() == false) {
                iter->second->Update(Clock::Get().ElapsedTime());
            }
            ++iter;
        }

        //Clean Up
        float cycleTime = Clock::Get().ElapsedTime() - m_cleanCyclestartTime;
        if (cycleTime >= m_cleanCycleIter) {
            for (auto iter = m_particleMap.begin(); iter != m_particleMap.end();)
            {
                if (iter->second->IsPendingDestroy() == true) {
                //    LOG("%s  PARTICLE REMOVING ", iter->second->GetID().c_str());
                    iter = m_particleMap.erase(iter);
                }
                else {
                    ++iter;
                }
            }
            m_cleanCyclestartTime = Clock::Get().ElapsedTime();
        }
    }
#pragma endregion

#pragma region HELPER METHODS

    void ParticleManager::AddParticleComp(std::string& _id, Particle* _anim)
    {
        auto found = m_particleMap.find(_id);
        if (found != m_particleMap.end()) {
            Logger::Get()->Trace(std::format("ERROR ! COMPONENT WITH ID {} ALREADY EXIST", _id));
            return;
        }
        m_particleMap.insert({ _id, _anim });
    }

    Particle* ParticleManager::GetParticle(std::string& _id) {
        auto found = m_particleMap.find(_id);
        if (found != m_particleMap.end()) {
            return found->second;
        }

        return  nullptr;
    }

    void cart::ParticleManager::RemoveParticle(std::string& _id)
    {
        auto found = m_particleMap.find(_id);
        if (found != m_particleMap.end()) {
            found->second->Destroy();
        }

    }

    void cart::ParticleManager::ParticleCompleteHandler(std::string _anim, bool _destroy)
    {

    }
#pragma endregion
}