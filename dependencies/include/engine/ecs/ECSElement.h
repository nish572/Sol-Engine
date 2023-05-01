#pragma once

#include <memory>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class ECSElement
{
public:
    ECSElement();
    ~ECSElement();

    EntityManager& getEntityManager();
    ComponentManager& getComponentManager();
    SystemManager& getSystemManager();

private:
    EntityManager m_entityManager;
    ComponentManager m_componentManager;
    SystemManager m_systemManager;
};
