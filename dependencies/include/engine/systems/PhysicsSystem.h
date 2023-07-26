#pragma once

#include <memory>

//#include <glad.h>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "ecs/Components.h"

using Entity = std::uint32_t;

//Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
}

namespace EcsPhysicsSystem
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        ~PhysicsSystem();

        bool initialize();

        void update(double deltaTime);

        void fixedUpdate(double fixedTimestep);;

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;
        std::unique_ptr<b2World> m_world;
    };
}
