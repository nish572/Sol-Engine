#include "event/EventElement.h"

#include "Core.h"

namespace CoreEventElement
{
	//EventElement has initializer list
	//All Elements MUST have at least m_core private member
	//If EventElement has private member(s) for pointer(s) to object(s) managed by EventElement
	//then include in initializer list here with nullptr as value(s)
	EventElement::EventElement(std::shared_ptr<Sol::Core> core) : m_core(core),  m_running(true)//Extend initializer list if necessary
	{
	}
	EventElement::~EventElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
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

	//Check if event element is running
	bool EventElement::isRunning() const
	{
		return m_running;
	}

	//Handle events
	void EventElement::handleEvents(const std::vector<SDL_Event>& events)
	{
		for (const auto& event : events)
		{
			processEvent(event);
		}
	}

	//Process event
	void EventElement::processEvent(const SDL_Event& event)
	{		
		if (ImGui_ImplSDL2_ProcessEvent(&event))
		{
			//Event is related to ImGui, so we can skip it
			//Check if the event belongs to ImGui
			if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)
			{
				return;
			}
		}

		//Check for application close events
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
			m_inputEvents.push_back(event);
		}
	}

	std::vector<SDL_Event> EventElement::getInputEvents()
	{
		return m_inputEvents;
	}

	void EventElement::resetInputEvents()
	{
		m_inputEvents.clear();
	}

	void EventElement::setInputsForPhysics(std::vector<std::shared_ptr<InputComponent>> inputs)
	{
		m_inputsForPhysics = inputs;
	}

	std::vector<std::shared_ptr<InputComponent>> EventElement::getInputsForPhysics()
	{
		return m_inputsForPhysics;
	}

	void EventElement::terminate()
	{
	}
}