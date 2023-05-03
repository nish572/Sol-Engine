#include "ecs/EcsElement.h"
#include "ecs/Components.h"

#include "Core.h"

// INCLUDE ANY SYSTEM.H HERE
// INCLUDE ANY COMPONENT .H HERE

namespace CoreEcsElement
{
    EcsElement::EcsElement(std::shared_ptr<Sol::Core> core) : m_core(core) {
        //Initialize data structures and systems
        //Add any necessary systems here (e.g., RenderSystem, PhysicsSystem, etc.)
        //m_systems[std::type_index(typeid(YourSystem))] = std::make_shared<YourSystem>(); // REPLACE WITH NAME OF SYSTEM E.G. RENDER SYSTEM
    }

    EcsElement::~EcsElement() {
        //Clean up resources
        terminate();
    }

    bool EcsElement::initialize() {
        //Initialize the EcsElement

        //Access other elements as needed
        auto corePtr = m_core.lock();
        if (corePtr)
        {
            m_resourceElement = corePtr->getResourceElement();
            m_renderElement = corePtr->getRenderElement();
            m_eventElement = corePtr->getEventElement();
            return true;
        }

        return false;
    }

    void EcsElement::update(double deltaTime) {
        //Call updateSystems(deltaTime) to update all systems
        updateSystems(deltaTime);
    }

    void EcsElement::fixedUpdate(double fixedTimestep) {
        //Call fixedUpdateSystems(fixedTimestep) to update fixed update systems
        fixedUpdateSystems(fixedTimestep);
    }

    void EcsElement::terminate() {
        //Clean up any resources associated with the ECSElement
        //Clear the entity-component map
        m_entityComponentMap.clear();
        //Clear the system maps
        m_systems.clear();
        m_fixedUpdateSystems.clear();
    }

    Entity EcsElement::createEntity() {
        // Get a new entity ID
        Entity entity = m_entityID++;
        return entity;
    }

    void EcsElement::destroyEntity(Entity entity) {
        //Remove all components associated with the entity
        for (auto it = m_entityComponentMap[entity].begin(); it != m_entityComponentMap[entity].end(); )
        {
            //Remove the component from the entity-component map
            it = m_entityComponentMap[entity].erase(it);
        }
        //Erase the entity from the entity-component map
        m_entityComponentMap.erase(entity);
    }

    template<typename T>
    void EcsElement::addComponent(Entity entity, T component) {
        //Get the type index for the component
        std::type_index componentTypeIndex(typeid(T));
        //Add the component to the entity-component map
        m_entityComponentMap[entity][componentTypeIndex] = std::make_shared<T>(component);
    }

    template<typename T>
    void EcsElement::removeComponent(Entity entity) {
        //Get the type index for the component
        std::type_index componentTypeIndex(typeid(T));
        //Remove the component from the entity-component map
        m_entityComponentMap[entity].erase(componentTypeIndex);
    }

    template<typename T>
    T& EcsElement::getComponent(Entity entity) {
        //Get the type index for the component
        std::type_index componentTypeIndex(typeid(T));
        //Get the component from the entity-component map
        return *std::static_pointer_cast<T>(m_entityComponentMap[entity][componentTypeIndex]);
    }

    template<typename T, typename... Args>
    void EcsElement::registerSystem(Args&&... args) {
        //Wrap the system in a lambda that casts the generic shared_ptr to the correct type
        auto system = std::make_shared<T>(std::forward<Args>(args)...);
        m_systems[std::type_index(typeid(T))] = [system](double deltaTime) {
            system->update(deltaTime);
        };
        m_fixedUpdateSystems[std::type_index(typeid(T))] = [system](double fixedTimestep) {
            system->fixedUpdate(fixedTimestep);
        };
    }

    void EcsElement::updateSystems(double deltaTime) {
        for (const auto& system : m_systems) {
            system.second(deltaTime);
        }
    }

    void EcsElement::fixedUpdateSystems(double fixedTimestep) {
        for (const auto& system : m_fixedUpdateSystems) {
            system.second(fixedTimestep);
        }
    }

    //Explicit template instantiation for the components and systems using (FOR EACH COMPONENT WRITE THE FOLLOWING)
    //REPLACE YOURCOMPONENT TYPE WITH THE NAME OF THE COMPONENT TYPE
    //template void EcsElement::addComponent<YourComponentType>(Entity entity, YourComponentType component);
    //template void EcsElement::removeComponent<YourComponentType>(Entity entity);
    //template YourComponentType& EcsElement::getComponent<YourComponentType>(Entity entity);
    template void EcsElement::addComponent<SpriteComponent>(Entity entity, SpriteComponent spriteComponent);
    template void EcsElement::removeComponent<SpriteComponent>(Entity entity);
    template SpriteComponent& EcsElement::getComponent<SpriteComponent>(Entity entity);

    //Explicit template instantiation for Each System (FOR EACH SYSTEM WRITE THE FOLLOWING)
    //template void EcsElement::registerSystem<YourSystem>(/*args*/); // args for the system constructor
}
