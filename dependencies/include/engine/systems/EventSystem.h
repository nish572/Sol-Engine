#pragma once

#include <memory>
#include <iostream>
#include <string>

#include "ecs/Components.h"

using Entity = std::uint32_t;

//Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
}

namespace EcsEventSystem
{
    class EventSystem
    {
    public:
        EventSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        ~EventSystem();

        void initialize();

        void update(double deltaTime);

        void fixedUpdate(double fixedTimestep);

        std::vector <std::shared_ptr<InputComponent>> getCurrentInputs();

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;

        std::vector<SDL_Event> m_inputEvents;
        std::vector <std::shared_ptr<InputComponent>> m_currentInputs;
    };
}
