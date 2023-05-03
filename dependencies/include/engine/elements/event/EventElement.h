#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <imgui_impl_sdl2.h>

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreEventElement
{
	//Represents the element responsible for managing the Event operations in the Sol Engine
	class EventElement
	{
	public:
		//Instantiate EventElement
		EventElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the EventElement instance
		~EventElement();

		//Initialize EventElement
		//Call this after calling EventElement's attachElement
		ENGINE_API bool initialize();

		//Check if SDL/ImGui events still need to be checked
		ENGINE_API bool isRunning() const;

		//Handle events
		void handleEvents(const std::vector<SDL_Event>& events);

		//Process events
		void processEvent(const SDL_Event& event);

		//Terminate EventElement
		//Call this to deallocate any of EventElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		bool m_running;
	};
}