#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreGUIElement
{
	//Represents the element responsible for managing the GUI operations in the Sol Engine
	class GUIElement
	{
	public:
		//Instantiate GUIElement
		GUIElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the GUIElement instance
		~GUIElement();

		//Initialize GUIElement
		//Call this after calling GUIElement's attachElement
		ENGINE_API bool initialize();

		void update(double deltaTime);

		void updateImGuiWindows();

		//Terminate GUIElement
		//Call this to deallocate any of GUIElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
	};
}