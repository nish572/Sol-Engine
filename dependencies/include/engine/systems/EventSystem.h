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

namespace EcsEventSystem
{
    class EventSystem
    {
    public:
        EventSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        ~EventSystem();

        void initialize();

        //Update is not used since event is dependent upon a fixed timesep
        void update(double deltaTime);

        //Fixed update is used since event is dependent upon a fixed timesep
        void fixedUpdate(double fixedTimestep);

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement; //Shared pointer of the ECS Element to allow the Event System to manage the ECS's lifetime otherwise the ECS may terminate before the events are finished

        std::vector<SDL_Event> m_inputEvents; //All events currently being input by the user
    };
}