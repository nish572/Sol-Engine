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

namespace CoreInputElement
{
	//State functionality of Element in a comment here
	class InputElement
	{
	public:
		//Represents the element responsible for managing the Input operations in the Sol Engine
		InputElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the InputElement instance
		~InputElement();

		//Initialize InputElement
		//Call this after calling InputElement's attachElement
		ENGINE_API bool initialize();

		ENGINE_API bool isRunning() const;

		void handleEvents(const std::vector<SDL_Event>& events);

		//Process events
		void processEvent(const SDL_Event& event);

		//Terminate InputElement
		//Call this to deallocate any of InputElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		bool m_running;
	};
}