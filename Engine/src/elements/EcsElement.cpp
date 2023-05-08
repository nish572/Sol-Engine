#include "ecs/EcsElement.h"
#include "ecs/Components.h"
//#include any systems here
#include "systems/RenderSystem.h"

#include "Core.h"

namespace CoreEcsElement
{
    EcsElement::EcsElement(std::shared_ptr<Sol::Core> core) : m_core(core)
    {
        
    }

    EcsElement::~EcsElement() {
        //Clean up resources
        terminate();
    }

    bool EcsElement::initialize() {
        //Initialize the EcsElement
        //Initialize data structures and systems
        //Add any necessary systems here (e.g., RenderSystem, PhysicsSystem, etc.)
        registerSystem<EcsRenderSystem::RenderSystem>(shared_from_this());
        //Access other elements as needed
        auto corePtr = m_core.lock();
        if (corePtr)
        {
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
        //Get a new entity ID
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
        std::type_index componentTypeIndex(typeid(T));
        m_entityComponentMap[entity][componentTypeIndex] = std::make_shared<T>(component);

        //Add the entity and component pointer to the componentEntityMap
        m_componentEntityMap[componentTypeIndex].push_back({ entity, m_entityComponentMap[entity][componentTypeIndex] });
    }

    template<typename T>
    void EcsElement::removeComponent(Entity entity) {
        std::type_index componentTypeIndex(typeid(T));
        m_entityComponentMap[entity].erase(componentTypeIndex);

        //Remove the entity and component pointer from the componentEntityMap
        auto& componentEntityList = m_componentEntityMap[componentTypeIndex];
        componentEntityList.erase(std::remove_if(componentEntityList.begin(), componentEntityList.end(),
            [&](const auto& pair) { return pair.first == entity; }),
            componentEntityList.end());
    }

    //Return reference to the specified component type associated with the given entity
    template<typename T>
    T& EcsElement::getComponent(Entity entity) {
        std::type_index componentTypeIndex(typeid(T));
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

    template <typename T>
    std::vector<std::pair<Entity, std::shared_ptr<T>>> EcsElement::getAllComponentsOfType() {
        std::type_index componentTypeIndex(typeid(T));
        std::vector<std::pair<Entity, std::shared_ptr<T>>> components;

        for (const auto& pair : m_componentEntityMap[componentTypeIndex]) {
            components.emplace_back(pair.first, std::static_pointer_cast<T>(pair.second));
        }

        return components;
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

    std::shared_ptr<Sol::Core> EcsElement::getCore()
    {
        auto corePtr = m_core.lock();
        if (corePtr)
        {
            return corePtr;
        }
        else
        {
            return nullptr;
        }
    }

    //Explicit template instantiation for the components
    template void EcsElement::addComponent<TransformComponent>(Entity entity, TransformComponent transformComponent);
    template void EcsElement::removeComponent<TransformComponent>(Entity entity);
    template TransformComponent& EcsElement::getComponent<TransformComponent>(Entity entity);
    template std::vector<std::pair<Entity, std::shared_ptr<TransformComponent>>> EcsElement::getAllComponentsOfType<TransformComponent>();

    template void EcsElement::addComponent<::SpriteComponent>(Entity entity, ::SpriteComponent spriteComponent);
    template void EcsElement::removeComponent<::SpriteComponent>(Entity entity);
    template ::SpriteComponent& EcsElement::getComponent<::SpriteComponent>(Entity entity);
    template std::vector<std::pair<Entity, std::shared_ptr<::SpriteComponent>>> EcsElement::getAllComponentsOfType<::SpriteComponent>();

    template void EcsElement::addComponent<ColliderComponent>(Entity entity, ColliderComponent colliderComponent);
    template void EcsElement::removeComponent<ColliderComponent>(Entity entity);
    template ColliderComponent& EcsElement::getComponent<ColliderComponent>(Entity entity);
    template std::vector<std::pair<Entity, std::shared_ptr<ColliderComponent>>> EcsElement::getAllComponentsOfType<ColliderComponent>();

    template void EcsElement::addComponent<PhysicsBodyComponent>(Entity entity, PhysicsBodyComponent colliderComponent);
    template void EcsElement::removeComponent<PhysicsBodyComponent>(Entity entity);
    template PhysicsBodyComponent& EcsElement::getComponent<PhysicsBodyComponent>(Entity entity);
    template std::vector<std::pair<Entity, std::shared_ptr<PhysicsBodyComponent>>> EcsElement::getAllComponentsOfType<PhysicsBodyComponent>();
}