//------- ECS Element -------
//An Entity-Component-System
//For The Sol Core Engine
//---------------------------

#include "ecs/EcsElement.h"
//#include any Systems here
#include "systems/RenderSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/EventSystem.h"

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

    //Call after Core's attachElement(elementName) has been called
    //Pass any required parameters for initialization, true/false for to indicate the presence of the three implemented Systens: render, physics, event
    bool EcsElement::initialize(bool renderSys, bool physSys, bool eventSys) {
        //Initialize the EcsElement
        auto corePtr = m_core.lock();
        if (corePtr)
        {
            //Register each System, if being used
            if (renderSys)
            {
                registerSystem<EcsRenderSystem::RenderSystem>(shared_from_this());
            }
            if (eventSys) {
                registerSystem<EcsEventSystem::EventSystem>(shared_from_this());
            }
            if (physSys) {
                registerSystem<EcsPhysicsSystem::PhysicsSystem>(shared_from_this());
            }
            return true;
        }
        return false;
    }

    //Get the Core so the Systems can access Elements
    std::shared_ptr<Sol::Core> EcsElement::getCore()
    {
        auto corePtr = m_core.lock();
        return corePtr;
    }
    
    //Call updateSystems(deltaTime) to update all Systems with a variable delta timestep
    void EcsElement::update(double deltaTime) {
        updateSystems(deltaTime);
    }

    //Call fixedUpdateSystems(fixedTimestep) to update all Systems with a fixed timestep
    void EcsElement::fixedUpdate(double fixedTimestep) {
        //Only update these Systems if the ECS is running, otherwise only rendering will be executed
        //Useful for having a playtest window that will still render but not simulate physics and vary with user game input or other events
        if (m_isSceneRunning) 
        {
            fixedUpdateSystems(fixedTimestep);
        }
    }

    //Clean up any resources associated with the ECS Element
    void EcsElement::terminate() {
        //Clear the ECS's data structures
        clear();
        //Clear the System maps
        m_systems.clear();
        m_fixedUpdateSystems.clear();
    }

    //Create a new Entity by returning the last Entity incremented by 1
    Entity EcsElement::createEntity() {
        //Get a new entity ID
        return m_entityID++;
    }

    //Destroy an Entity by clearing its resources
    void EcsElement::destroyEntity(Entity entity) {
        //Remove all components associated with the Entity
        for (auto it = m_entityComponentMap[entity].begin(); it != m_entityComponentMap[entity].end(); )
        {
            //Remove the component from the entity-component map
            it = m_entityComponentMap[entity].erase(it);
        }
        //Erase the entity from the entity-component map
        m_entityComponentMap.erase(entity);
        //Could also reduce m_entityID by 1
        //Leaving as is because one may want the ID of the deleted Entity to remain
        //No harm in this since creating an entity only returns the result of m_entityID++, which is always a std::uint32_t
    }

    //Return the entity-component map
    std::unordered_map<Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> EcsElement::getEntityMap()
    {
        return m_entityComponentMap;
    }

    //Set the running state of the scene, which determines whether or not variable delta timestep dependent Systems should be executed
    void EcsElement::setSceneRunning(bool runningState)
    {
        m_isSceneRunning = runningState;
    }

    //Clear the ECS's data structures
    void EcsElement::clear()
    {
        m_entityComponentMap.clear();
        m_componentEntityMap.clear();
        //Also reset Entity ID to 0
        m_entityID = 0;
    }

    //Update all Systems that use a variable delta timestep
    void EcsElement::updateSystems(double deltaTime) {
        for (const auto& system : m_systems) {
            system.second(deltaTime);
        }
    }

    //Update all Systems that use a fixed timestep
    void EcsElement::fixedUpdateSystems(double fixedTimestep) {
        for (const auto& system : m_fixedUpdateSystems) {
            system.second(fixedTimestep);
        }
    }
} 