#pragma once

#include "EngineAPI.h"

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>

#include "resource/ResourceElement.h"
#include "render/RenderElement.h"
#include "event/EventElement.h"

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

        // Entity management functions
        Entity createEntity();
        void destroyEntity(Entity entity);

        // Component management functions
        template<typename T>
        void addComponent(Entity entity, T component);
        template<typename T>
        void removeComponent(Entity entity);
        template<typename T>
        T& getComponent(Entity entity);
        template <typename T>
        std::vector<std::pair<Entity, std::shared_ptr<T>>> getAllComponentsOfType();

        // System management functions
        template<typename T, typename... Args>
        void registerSystem(Args&&... args);
        void updateSystems(double deltaTime);
        void fixedUpdateSystems(double fixedTimestep);

    private:
        //Pointer to Core
        std::weak_ptr<Sol::Core> m_core;
        //Is LogElement present
        bool m_logElementAttached{ false };

        // Data structures for entity-component-system management
        std::vector<Entity> m_entities;
        std::unordered_map<Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> m_entityComponentMap;
        std::unordered_map<std::type_index, std::vector<std::pair<Entity, std::shared_ptr<void>>>> m_componentEntityMap;
        std::unordered_map<std::type_index, std::function<void(double)>> m_systems;
        std::unordered_map<std::type_index, std::function<void(double)>> m_fixedUpdateSystems;

        // Entity ID management
        Entity m_entityID = 0;

        // Pointer for each element
        CoreResourceElement::ResourceElement* m_resourceElement;
        CoreRenderElement::RenderElement* m_renderElement;
        CoreEventElement::EventElement* m_eventElement;
    };
}
