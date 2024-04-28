//------- Events System -----
//Implements Events Processing
//For The Sol Core Engine
//---------------------------

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
        //Not necessary for the Event System as it doesn't require frame-dependent updates
    }

    //Prepare actions based upon key event for the Physics System to utilise to apply forces/impulses/torques
    void EventSystem::fixedUpdate(double fixedTimestep)
    {
        //Get all input events and all input components to find key-relevant actions
        m_inputEvents = m_ecsElement->getCore()->getEventElement()->getInputEvents();
        auto inputComponents = m_ecsElement->getAllComponentsOfType<InputComponent>();

        //Structure to hold the specific actions to be executed for each entity based on the pressed keys
        std::unordered_map<Entity, std::vector<ActionData>> actionsPerEntity;

        for (const auto& event : m_inputEvents)
        {
            SDL_Keycode eventKey = event.key.keysym.sym;
            for (const auto& inputPair : inputComponents)
            {
                auto entity = inputPair.first;
                auto& inputComponent = inputPair.second;

                //Find if this input component has an action mapped to the eventKey
                auto actionIt = inputComponent->keyActions.find(eventKey);
                if (actionIt != inputComponent->keyActions.end())
                {
                    //If so, add all actions mapped to this key to the actionsPerEntity for the entity
                    const std::vector<ActionData>& actions = actionIt->second;
                    actionsPerEntity[entity].insert(actionsPerEntity[entity].end(), actions.begin(), actions.end());
                }
            }
        }

        //Pass the actionsPerEntity to the Physics System
        //Or rather, set a data structure the Physics System can use to access these currently induced actions
        m_ecsElement->getCore()->getEventElement()->setActionsForPhysics(actionsPerEntity);
    }
}