#pragma once

#include "EngineAPI.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>

#include "resource/ResourceElement.h"
#include "render/RenderElement.h"
#include "event/EventElement.h"
#include "ecs/Components.h"

namespace Sol {
    class Core;
}

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 5000;

namespace CoreEcsElement
{
    class EcsElement : public std::enable_shared_from_this<EcsElement> {
    public:
        EcsElement(std::shared_ptr<Sol::Core> core);
        ~EcsElement();

        ENGINE_API bool initialize();
        void update(double deltaTime);
        void fixedUpdate(double fixedTimestep);
        void terminate();

        //Entity management functions
        ENGINE_API Entity createEntity();
        void destroyEntity(Entity entity);

        //Component management functions
        template<typename T>
        void addComponent(Entity entity, T component) {
            std::type_index componentTypeIndex(typeid(T));
            m_entityComponentMap[entity][componentTypeIndex] = std::make_shared<T>(component);

            //Add the entity and component pointer to the componentEntityMap
            m_componentEntityMap[componentTypeIndex].push_back({ entity, m_entityComponentMap[entity][componentTypeIndex] });
        }

        template<typename T>
        void removeComponent(Entity entity) {
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
        T& getComponent(Entity entity) {
            std::type_index componentTypeIndex(typeid(T));
            return *std::static_pointer_cast<T>(m_entityComponentMap[entity][componentTypeIndex]);
        }

        template <typename T>
        std::vector<std::pair<Entity, std::shared_ptr<T>>> getAllComponentsOfType() {
            std::type_index componentTypeIndex(typeid(T));
            std::vector<std::pair<Entity, std::shared_ptr<T>>> components;

            for (const auto& pair : m_componentEntityMap[componentTypeIndex]) {
                components.emplace_back(pair.first, std::static_pointer_cast<T>(pair.second));
            }

            return components;
        }

        //System management functions
        template<typename T, typename... Args>
        void registerSystem(Args&&... args)
        {
            //Wrap the system in a lambda that casts the generic shared_ptr to the correct type
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            m_systems[std::type_index(typeid(T))] = [system](double deltaTime) {
                system->update(deltaTime);
                //
            };
            m_fixedUpdateSystems[std::type_index(typeid(T))] = [system](double fixedTimestep) {
                system->fixedUpdate(fixedTimestep);
                //
            };
        }

        void updateSystems(double deltaTime);
        void fixedUpdateSystems(double fixedTimestep);

        std::shared_ptr<Sol::Core> getCore();

        //Sprite functions
        ENGINE_API void addSprite(Entity entity) {
            SpriteComponent tmp = SpriteComponent{};
            addComponent<SpriteComponent>(entity, tmp);
        }
        ENGINE_API void removeSprite(Entity entity) {removeComponent<SpriteComponent>(entity);}
        ENGINE_API SpriteComponent& getSprite(Entity entity) {return getComponent<SpriteComponent>(entity);}

        //Transform functions
        ENGINE_API void addTransform(Entity entity) {
            TransformComponent tmp = TransformComponent{};
            addComponent<TransformComponent>(entity, tmp);
        }
        ENGINE_API void removeTransform(Entity entity) {removeComponent<TransformComponent>(entity);}
        ENGINE_API TransformComponent& getTransform(Entity entity) {return getComponent<TransformComponent>(entity);}

    private:
        //Pointer to Core
        std::weak_ptr<Sol::Core> m_core;

        //Data structures for entity-component-system management
        std::unordered_set<Entity> m_entities;
        std::unordered_map<Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> m_entityComponentMap;
        std::unordered_map<std::type_index, std::vector<std::pair<Entity, std::shared_ptr<void>>>> m_componentEntityMap;
        std::unordered_map<std::type_index, std::function<void(double)>> m_systems;
        std::unordered_map<std::type_index, std::function<void(double)>> m_fixedUpdateSystems;

        //Entity ID management
        Entity m_entityID = 0;
    };
}
