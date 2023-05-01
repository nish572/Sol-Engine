#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
//Base class of all systems in the ECS framework
#include "BaseSystem.h"

class SystemManager
{
public:
    //addSystem adds a system to the map
    template <typename T, typename... Args>
    void addSystem(Args&&... args);
    //getSystem returns a system from the map
    template <typename T>
    //Function template that returns a reference to a system of type T and stores it in the m_systems map
    T& getSystem();
    //removeSystem removes a system from the map
    template <typename T>
    void removeSystem();
    //update calls the update function of each system
    void update(double deltaTime);

private:
    //data structure that holds systems, indexed by type
    std::unordered_map<std::type_index, std::unique_ptr<BaseSystem>> m_systems;
};
