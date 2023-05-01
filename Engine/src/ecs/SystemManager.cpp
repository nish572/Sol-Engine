#include "ecs/SystemManager.h"

template <typename T, typename... Args>
void SystemManager::addSystem(Args&&... args)
{
    m_systems[std::type_index(typeid(T))] = std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
T& SystemManager::getSystem()
{
    return *static_cast<T*>(m_systems.at(std::type_index(typeid(T))).get());
}

template <typename T>
void SystemManager::removeSystem()
{
    m_systems.erase(std::type_index(typeid(T)));
}

void SystemManager::update(double deltaTime)
{
    //iterate through all systems and call their update functions
    for (auto& systemPair : m_systems)
    {
        //systemPair is a pair of a type index and a unique pointer to a system
        auto& system = systemPair.second;
        //system is a unique pointer to a system
        system->update(deltaTime);
    }
}

void SystemManager::fixedUpdate(double fixedTimestep)
{
    for (auto& systemPair : m_systems)
    {
        auto& system = systemPair.second;
        system->fixedUpdate(fixedTimestep);
    }
}