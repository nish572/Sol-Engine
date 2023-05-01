#include "EntityManager.h"

EntityManager::Entity EntityManager::createEntity()
{
    //Entity is just an unsigned integer
    Entity entity;

    //Check if there are any available entities
    if (!m_availableEntities.empty())
    {
        //If there are, get the first available entity
        entity = m_availableEntities.front();
        //Pop the entity from the queue
        m_availableEntities.pop();
        //Set the entity to be alive
        m_entityAlive[entity] = true;
    }
    else
    {
        //If there are no available entities, create a new one
        //Increments m_nextEntity to get next available entity ID
        entity = m_nextEntity++;
        //Set the entity to be alive
        m_entityAlive.push_back(true);
    }
    //Return the entity
    return entity;
}

void EntityManager::destroyEntity(Entity entity)
{
    //Check if the entity is alive
    if (isEntityAlive(entity))
    {
        //If it is, set it to be dead
        m_entityAlive[entity] = false;
        //Push the entity to the queue of available entities
        m_availableEntities.push(entity);
    }
}

bool EntityManager::isEntityAlive(Entity entity) const
{
    //Check if the entity is alive
    return m_entityAlive[entity];
}
