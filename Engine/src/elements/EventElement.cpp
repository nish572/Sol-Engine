//------- Event Element -----
//Manages User Input
//For The Sol Core Engine
//---------------------------

#include "event/EventElement.h"

#include "Core.h"

namespace CoreEventElement
{
	EventElement::EventElement(std::shared_ptr<Sol::Core> core) : m_core(core),  m_running(true)
	{
	}
	EventElement::~EventElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, none
	bool EventElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}
		if (corePtr)
		{
			if (!corePtr->getRenderElement())
			{
				//The Event Element requires the Render Element for processing events through SDL
				//If not present, then log/output the appropriate error
				if (m_logElementAttached)
				{
					corePtr->getLogElement()->logError("[Event] Failed to initialize EventElement: RenderElement is a nullptr");
					return false;
				}
				std::cerr << "Failed to initialize EventElement: RenderElement is a nullptr" << std::endl;
				return false;
			}
		}

		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Event] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Event] Successfully Initialized" << std::endl;
		return true;
	}

	//Check if Event Element is running
	bool EventElement::isRunning() const
	{
		return m_running;
	}

	//Handle events by processing the queue
	void EventElement::handleEvents(const std::vector<SDL_Event>& events)
	{
		for (const auto& event : events)
		{
			processEvent(event);
		}
	}

	//Process events
	void EventElement::processEvent(const SDL_Event& event)
	{
		//Event is related to ImGui, so we can skip it	
		if (ImGui_ImplSDL2_ProcessEvent(&event))
		{
			//Check if the event belongs to ImGui, and if so, skip
			if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)
			{
				return;
			}
		}

		//Check for application close events, and if event present then set the running state to false to signal closing to the Core
		if (event.type == SDL_QUIT)
		{
			m_running = false;
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_core.lock()->getRenderElement()->getWindow()))
		{
			m_running = false;
		}

		//Check for keyboard events
		if (event.type == SDL_KEYDOWN)
		{
			//Push these back to a data structure of current input events
			m_inputEvents.push_back(event);
		}
	}

	//The following functions are to be used by the Event System and the Physics System
	//Their usage is to compare any current input events to existing Input Component Actions
	//And then for Physics to apply forces/impulses/torques by input
	std::vector<SDL_Event> EventElement::getInputEvents()
	{
		return m_inputEvents;
	}

	void EventElement::resetInputEvents()
	{
		m_inputEvents.clear();
	}

	void EventElement::setActionsForPhysics(const std::unordered_map<Entity, std::vector<ActionData>>& actionsPerEntity) {
		m_actionsForPhysics = actionsPerEntity;
	}

	const std::unordered_map<Entity, std::vector<ActionData>>& EventElement::getActionsForPhysics() const {
		return m_actionsForPhysics;
	}

	//Event Element cleanup, should be clean prior but still useful to clear as a failsafe
	void EventElement::terminate()
	{
		resetInputEvents();
		m_actionsForPhysics.clear();
	}
}