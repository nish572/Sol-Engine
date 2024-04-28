#pragma once

//C++ libraries
#include <memory>
#include <iostream>
#include <string>

#include "ecs/Components.h"

//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;

//Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
}

namespace Sol {
    class Core;
}

namespace ApplicationConfig
{
    class Config;
}

namespace EcsPhysicsSystem
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        ~PhysicsSystem();

        void initialize();

        //Update is not used since physics is dependent upon a fixed timesep
        void update(double deltaTime);

        //Fixed update is used since physics is dependent upon a fixed timesep
        void fixedUpdate(double fixedTimestep);

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement; //Shared pointer of the ECS Element to allow the Physics System to manage the ECS's lifetime otherwise the ECS may terminate before the physics is finished
        std::shared_ptr<b2World> m_world; //World definition
        std::unordered_map<BodyType, b2BodyDef> m_bodyDefs; //Body definitions for Box2D

        //Scaling factor for translation of Box2D coords into OpenGL appropriate
        const float m_scalingFactor = 100.0f; //100 pixels per metre
    };
}