#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <memory>
#include <string>
#include <vector>

//The external libraries SDL2 and Dear ImGui
#include <SDL.h>
#include <imgui_impl_sdl2.h>

#include "ecs/Components.h"

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;

namespace CoreEventElement
{
	//Represents the Element responsible for managing the Event operations in the Sol Engine
	//Primarily keyboard/mouse input
	class EventElement
	{
	public:
		EventElement(std::shared_ptr<Sol::Core> core);
		~EventElement();

		ENGINE_API bool initialize();

		//Check if SDL/ImGui events still need to be checked
		ENGINE_API bool isRunning() const;

		//Handle events
		void handleEvents(const std::vector<SDL_Event>& events);

		//Process events
		void processEvent(const SDL_Event& event);

		//Get all input events
		std::vector<SDL_Event> getInputEvents();

		void resetInputEvents();

		//For use by the Physics System so forces/impulses/torques can be applied by key input
		void setActionsForPhysics(const std::unordered_map<Entity, std::vector<ActionData>>& actionsPerEntity);
		const std::unordered_map<Entity, std::vector<ActionData>>& getActionsForPhysics() const;

		//Call terminate to deallocate any of Event Element's resources
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		//Used by the Core to determine whether or not any SDL/ImGui quit events have been detected, and if so then the application cleanly quits
		bool m_running;
		//For use to determine and send any events required in-scene
		std::vector<SDL_Event> m_inputEvents;
		std::unordered_map<Entity, std::vector<ActionData>> m_actionsForPhysics;
	};
}