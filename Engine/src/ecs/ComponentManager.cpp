#include "ComponentManager.h"

template <typename T>
//addComponent adds a component of type T to an entity
void ComponentManager::addComponent(EntityManager::Entity entity, T component)
{
    //m_components is a map of vectors of pairs of entities and void pointers
    //emplace_back constructs a new element at the end of the vector
    m_components[std::type_index(typeid(T))].emplace_back(entity, new T(component));
}

template <typename T>
//getComponent returns a component of type T from an entity
T& ComponentManager::getComponent(EntityManager::Entity entity)
{
    //retrieves vector of components of type T for all entities
    auto& components = m_components[std::type_index(typeid(T))];
    //std::find_if searches for an element in a range for which a predicate is true
    auto it = std::find_if(components.begin(), components.end(),
        //lambda expression that returns true if the entity of the current element is equal to the entity passed to the function
        [entity](const auto& element) { return element.first == entity; });

    //if the component is found, return it
    if (it != components.end())
    {
        //static_cast converts a void pointer to a pointer of type T
        return *static_cast<T*>(it->second);
    }

    //Handle component not found, e.g., throw an exception or return a default component
}

template <typename T>
//removeComponent removes a component of type T from an entity
void ComponentManager::removeComponent(EntityManager::Entity entity)
{
    //retrieves vector of components of type T for all entities
    auto& components = m_components[std::type_index(typeid(T))];
    //std::find_if searches for an element in a range for which a predicate is true
    auto it = std::find_if(components.begin(), components.end(),
        //lambda expression that returns true if the entity of the current element is equal to the entity passed to the function
        [entity](const auto& element) { return element.first == entity; });
    //if the component is found, delete it and remove it from the vector
    if (it != components.end())
    {
        //static_cast converts a void pointer to a pointer of type T
        delete static_cast<T*>(it->second);
        //std::vector::erase removes an element from a vector
        components.erase(it);
    }
}
