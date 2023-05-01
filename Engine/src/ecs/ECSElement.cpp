#include "ecs/ECSElement.h"

#include "Core.h"

namespace CoreECSElement
{
    ECSElement::ECSElement(std::shared_ptr<Sol::Core> core) : m_core(core), m_entityManager{}, m_componentManager{}, m_systemManager{}
    {
    }

    ECSElement::~ECSElement()
    {
    }

    EntityManager& ECSElement::getEntityManager()
    {
        return m_entityManager;
    }

    ComponentManager& ECSElement::getComponentManager()
    {
        return m_componentManager;
    }

    SystemManager& ECSElement::getSystemManager()
    {
        return m_systemManager;
    }
}