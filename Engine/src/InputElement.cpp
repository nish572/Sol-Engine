#include "input/InputElement.h"

#include "Core.h"

namespace CoreInputElement
{
	//InputElement has initializer list
	//All Elements MUST have at least m_core private member
	//If InputElement has private member(s) for pointer(s) to object(s) managed by InputElement
	//then include in initializer list here with nullptr as value(s)
	InputElement::InputElement(std::shared_ptr<Sol::Core> core) : m_core(core),  m_running(true)//Extend initializer list if necessary
	{
	}
	InputElement::~InputElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool InputElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (!corePtr->getLogElement())
			{
				std::cerr << "Failed to initialize InputElement: LogElement is a nullptr" << std::endl;
				return false;
			}
		}
		if (corePtr)
		{
			if (!corePtr->getRenderElement())
			{
				std::cerr << "Failed to initialize RenderElement: RenderElement is a nullptr" << std::endl;
				return false;
			}
		}

		if (corePtr)
		{
			corePtr->getLogElement()->logInfo("[Input] Successfully Initialized");
		}
		return true;
	}

	//Check if input element is running
	bool InputElement::isRunning() const
	{
		return m_running;
	}

	//Handle events
	void InputElement::handleEvents(const std::vector<SDL_Event>& events)
	{
		for (const auto& event : events)
		{
			processEvent(event);
		}
	}

	//Process event
	void InputElement::processEvent(const SDL_Event& event)
	{
		// Check if the event belongs to ImGui
		if (ImGui_ImplSDL2_ProcessEvent(&event))
		{
			//Event is related to ImGui, so we can skip it
			return;
		}

		// Check for application close events
		if (event.type == SDL_QUIT)
		{
			m_running = false;
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_core.lock()->getRenderElement()->getWindow()))
		{
			m_running = false;
		}
	}

	void InputElement::terminate()
	{
	}
}