#pragma once

#include <unordered_map>
#include <typeindex>
#include <vector>
#include "EntityManager.h"

//Defines the ComponentManager class
//Responsible for adding, retrieving, and removing components from entities
//Utility functions are templated to allow for any component type
//Data structure is a map of vectors of pairs of entities and void pointers

class ComponentManager
{
public:
    //T is a component type
    template <typename T>
    //entity is an entity to which the component will be added
    void addComponent(EntityManager::Entity entity, T component);

    //T is a component type
    template <typename T>
    //entity is an entity from which the component will be retrieved
    T& getComponent(EntityManager::Entity entity);

    //T is a component type
    template <typename T>
    //entity is an entity from which the component will be removed
    void removeComponent(EntityManager::Entity entity);

private:
    //data structure that holds components for all entities
    //std::type_index is a wrapper for std::type_info, and is used as a key representing the type of the component
    //value is a vector of pairs containing an entity and a pointer to its component of that type
    std::unordered_map<std::type_index, std::vector<std::pair<EntityManager::Entity, void*>>> m_components;
};
