#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>

#include "ecs/EntityManager.h"
#include "ecs/ComponentManager.h"
#include "ecs/SystemManager.h"

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency

namespace Sol {
    class Core;
}

namespace CoreECSElement
{
    //ECSElement represents the ECS Framework capabilities of the Sol Engine
    class ECSElement
    {
    public:
        //Construct ECS and pass shared_ptr to Core
        ECSElement(std::shared_ptr<Sol::Core> core);
        ~ECSElement();

        EntityManager& getEntityManager();
        ComponentManager& getComponentManager();
        SystemManager& getSystemManager();

    private:
        std::weak_ptr<Sol::Core> m_core;
        EntityManager m_entityManager;
        ComponentManager m_componentManager;
        SystemManager m_systemManager;
    };
}
