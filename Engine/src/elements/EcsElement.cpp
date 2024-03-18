#include "ecs/EcsElement.h"
//#include any systems here
#include "systems/RenderSystem.h"
#include "systems/PhysicsSystem.h"

#include "Core.h"

namespace CoreEcsElement
{
    EcsElement::EcsElement(std::shared_ptr<Sol::Core> core) : m_core(core)
    {
    }

    EcsElement::~EcsElement() {
        //Clean up resources
        terminate();
    }

    bool EcsElement::initialize(bool renderSys, bool physSys, bool eventSys) {
        //Initialize the EcsElement
        auto corePtr = m_core.lock();
        if (corePtr)
        {
            if (renderSys)
            {
                registerSystem<EcsRenderSystem::RenderSystem>(shared_from_this());
            }
            if (physSys) {
                registerSystem<EcsPhysicsSystem::PhysicsSystem>(shared_from_this());
            }
            return true;
        }
        return false;
    }

    void EcsElement::update(double deltaTime) {
        //Call updateSystems(deltaTime) to update all systems
        updateSystems(deltaTime);
    }

    void EcsElement::fixedUpdate(double fixedTimestep) {
        //Call fixedUpdateSystems(fixedTimestep) to update fixed update systems
        fixedUpdateSystems(fixedTimestep);
    }

    void EcsElement::terminate() {
        //Clean up any resources associated with the ECSElement
        //Clear the entity-component map
        m_entityComponentMap.clear();
        //Clear the system maps
        m_systems.clear();
        m_fixedUpdateSystems.clear();
    }

    Entity EcsElement::createEntity() {
        //Get a new entity ID
        return m_entityID++;
    }

    void EcsElement::destroyEntity(Entity entity) {
        //Remove all components associated with the entity
        for (auto it = m_entityComponentMap[entity].begin(); it != m_entityComponentMap[entity].end(); )
        {
            //Remove the component from the entity-component map
            it = m_entityComponentMap[entity].erase(it);
        }
        //Erase the entity from the entity-component map
        m_entityComponentMap.erase(entity);
        //Could also reduce m_entityID by 1
        //Leaving as is because one may want the ID of the deleted entity still available
        //No harm in this since creating an entity only returns the result of m_entityID++, which is always a std::uint32_t;
    }

    void EcsElement::updateSystems(double deltaTime) {
        for (const auto& system : m_systems) {
            system.second(deltaTime);
        }
    }

    void EcsElement::fixedUpdateSystems(double fixedTimestep) {
        for (const auto& system : m_fixedUpdateSystems) {
            system.second(fixedTimestep);
        }
    }
} 