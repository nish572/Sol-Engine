#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>

//Other Elements being used by the ECS/by Systems using the ECS
#include "resource/ResourceElement.h"
#include "render/RenderElement.h"
#include "physics/PhysicsElement.h"
#include "event/EventElement.h"
#include "ecs/Components.h"

//The only usage of global scope, used for project settings primarily
//However rendering and physics require the main viewport size, and the project path for browsing resources
#include "Config.h"

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
    class Core;
}

//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;
//const Entity MAX_ENTITIES = 5000; //Would be global which is unwanted, also currently not in use since the ECS is performative enough and introducing this many Entities is incredibly unlikely

namespace CoreEcsElement
{
    //Represents the Element responsible for managing the Entity-Component-System operations in the Sol Engine
    //This includes Entity and Component management
    //Additionally includes the management and updating of the Systems (both variable delta timestep, and fixed timestep)
    class EcsElement : public std::enable_shared_from_this<EcsElement> { //Allows the ECS Element to be owned by the Core and by the Systems
    public:
        EcsElement(std::shared_ptr<Sol::Core> core);
        ~EcsElement();

        ENGINE_API bool initialize(bool renderSys, bool physSys, bool eventSys);
        void update(double deltaTime);
        void fixedUpdate(double fixedTimestep);
        //Call terminate to deallocate all of the ECS's resources
        void terminate();

        std::shared_ptr<Sol::Core> getCore();

        //Entity management functions to create or destroy an Entity
        Entity createEntity();
        void destroyEntity(Entity entity);

        //Component management functions as templates to be defined per component
        template<typename T>
        void addComponent(Entity entity, T component) {
            std::type_index componentTypeIndex(typeid(T));
            m_entityComponentMap[entity][componentTypeIndex] = std::make_shared<T>(component);

            //Add the Entity and Component pointer to the componentEntityMap
            m_componentEntityMap[componentTypeIndex].push_back({ entity, m_entityComponentMap[entity][componentTypeIndex] });
        }

        template<typename T>
        void removeComponent(Entity entity) {
            std::type_index componentTypeIndex(typeid(T));
            m_entityComponentMap[entity].erase(componentTypeIndex);

            //Remove the Entity and Component pointer from the componentEntityMap
            auto& componentEntityList = m_componentEntityMap[componentTypeIndex];
            componentEntityList.erase(std::remove_if(componentEntityList.begin(), componentEntityList.end(),
                [&](const auto& pair) { return pair.first == entity; }),
                componentEntityList.end());
        }

        //Return the Component of the specified type associated with the given Entity
        template<typename T>
        T& getComponent(Entity entity) {
            std::type_index componentTypeIndex(typeid(T));
            return *std::static_pointer_cast<T>(m_entityComponentMap[entity][componentTypeIndex]);
        }

        //Return the map of all Components of the specified type
        template <typename T>
        std::unordered_map<Entity, std::shared_ptr<T>> getAllComponentsOfType() {
            std::type_index componentTypeIndex(typeid(T));
            std::unordered_map<Entity, std::shared_ptr<T>> components;

            for (const auto& pair : m_componentEntityMap[componentTypeIndex]) {
                components.emplace(pair.first, std::static_pointer_cast<T>(pair.second));
            }

            return components;
        }

        //System management functions
        template<typename T, typename... Args>
        void registerSystem(Args&&... args)
        {
            //Wrap the system in a lambda that casts the generic shared_ptr to the correct type
            //Minor workaround, but both an update and a fixedUpdate function are checked and added for each System
            //However, dependent on the System type, only one function should be populated in the System
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            m_systems[std::type_index(typeid(T))] = [system](double deltaTime) {
                system->update(deltaTime);
            };
            m_fixedUpdateSystems[std::type_index(typeid(T))] = [system](double fixedTimestep) {
                system->fixedUpdate(fixedTimestep);
            };
        }

        void updateSystems(double deltaTime);
        void fixedUpdateSystems(double fixedTimestep);

        void setSceneRunning(bool runningState);
        std::unordered_map<Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> getEntityMap();
        void clear();

        //Sprite functions using the templated Component management functions
        void addSprite(Entity entity) {
            SpriteComponent tmp = SpriteComponent{};
            addComponent<SpriteComponent>(entity, tmp);
        }
        void removeSprite(Entity entity) {removeComponent<SpriteComponent>(entity);}
        SpriteComponent& getSprite(Entity entity) {return getComponent<SpriteComponent>(entity);}

        //Transform functions using the templated Component management functions
        void addTransform(Entity entity) {
            TransformComponent tmp = TransformComponent{};
            addComponent<TransformComponent>(entity, tmp);
        }
        void removeTransform(Entity entity) {removeComponent<TransformComponent>(entity);}
        TransformComponent& getTransform(Entity entity) {return getComponent<TransformComponent>(entity);}

        //Body functions using the templated Component management functions
        void addPhysicsBody(Entity entity) {
            PhysicsBodyComponent tmp = PhysicsBodyComponent{};
            addComponent<PhysicsBodyComponent>(entity, tmp);
        }
        void removePhysicsBody(Entity entity) {removeComponent<PhysicsBodyComponent>(entity);}
        PhysicsBodyComponent& getPhysicsBody(Entity entity) {return getComponent<PhysicsBodyComponent>(entity);}

        //Collider functions using the templated Component management functions
        void addCollider(Entity entity) {
            ColliderComponent tmp = ColliderComponent{};
            addComponent<ColliderComponent>(entity, tmp);
        }
        void removeCollider(Entity entity) { removeComponent<ColliderComponent>(entity); }
        ColliderComponent& getCollider(Entity entity) { return getComponent<ColliderComponent>(entity); }

        //Input functions using the templated Component management functions
        void addInput(Entity entity) {
            InputComponent tmp = InputComponent{};
            addComponent<InputComponent>(entity, tmp);
        }
        void removeInput(Entity entity) { removeComponent<InputComponent>(entity); }
        InputComponent& getInput(Entity entity) { return getComponent<InputComponent>(entity); }

    private:
        //Pointer to Core
        std::weak_ptr<Sol::Core> m_core;

        //Data structures for Entity-Component-System management

        std::unordered_set<Entity> m_entities; //Currently not used
        //Stores the Entity as the key, with an unordered map as the value (where the key:value of this map is the Component type:a shared pointer of the Component)
        std::unordered_map<Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> m_entityComponentMap;
        //An unordered map such that the key is the Component type, the value is a vector
        //This vector holds pairs such that the key of the pair is the Entity and the value is the pointer to the Component of the type held by the key of the unordered map
        std::unordered_map<std::type_index, std::vector<std::pair<Entity, std::shared_ptr<void>>>> m_componentEntityMap;
        //Stores an unordered map such that the key is the type of the System, and the value is the update function (with parameter delta timestep) held by the System of that type
        std::unordered_map<std::type_index, std::function<void(double)>> m_systems;
        //Stores an unordered map such that the key is the type of the System, and the value is the update function (with parameter fixed timestep) held by the System of that type
        std::unordered_map<std::type_index, std::function<void(double)>> m_fixedUpdateSystems;

        //Entity ID management, starting at 0
        Entity m_entityID = 0;

        //The running state of the scene and therefore the fixed update state of the ECS
        bool m_isSceneRunning = true;
    };
}