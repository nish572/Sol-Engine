#pragma once

#include <vector>
#include <queue>
#include <cstdint>

//Defines the ComponentManager class
//Responsible for adding, retrieving, and removing entities in this ECS framework
//This includes the constructor and destructor, as well as the functions to create entities, delete entities, and check if an entity exists

class EntityManager
{
public:
    //Entity is just an unsigned integer
    using Entity = std::uint32_t;
    //Create entity, delete entity, check if entity is alive
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool isEntityAlive(Entity entity) const;

private:
    //Vector of booleans to check if entity is alive
    std::vector<bool> m_entityAlive;
    //Queue of available entities
    std::queue<Entity> m_availableEntities;
    //Next entity to be created
    Entity m_nextEntity;
};
