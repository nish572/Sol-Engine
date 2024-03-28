#include "systems/EventSystem.h"
#include "ecs/EcsElement.h"

#include "Core.h"

namespace EcsEventSystem
{
    EventSystem::EventSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
        : m_ecsElement(ecsElement)
    {
        initialize();
    }

    EventSystem::~EventSystem()
    {
    }

    void EventSystem::initialize()
    {
    }

    void EventSystem::update(double deltaTime)
    {
        //Not necessary for the EventSystem as it doesn't require frame-dependent updates
    }

    void EventSystem::fixedUpdate(double fixedTimestep)
    {
        m_inputEvents = m_ecsElement->getCore()->getEventElement()->getInputEvents();
        auto inputComponents = m_ecsElement->getAllComponentsOfType<InputComponent>();

        for (const auto& event : m_inputEvents)
        {
            SDL_Keycode eventKey = event.key.keysym.sym;
            for (const auto& inputPair : inputComponents) 
            {
                auto entity = inputPair.first;

                auto& inputComponent = inputPair.second;

                if (inputComponent->key == eventKey)
                {
                    m_currentInputs.push_back(inputComponent);
                }
            }
        }
        m_ecsElement->getCore()->getEventElement()->setInputsForPhysics(m_currentInputs);
    }

    std::vector <std::shared_ptr<InputComponent>> EventSystem::getCurrentInputs()
    {
        return m_currentInputs;
    }
}