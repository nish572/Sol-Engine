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

namespace CoreGuiElement
{
	//Represents the element responsible for managing the Gui operations in the Sol Engine
	class GuiElement
	{
	public:
		//Instantiate GuiElement
		GuiElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the GuiElement instance
		~GuiElement();

		//Initialize GuiElement
		//Call this after calling GuiElement's attachElement
		ENGINE_API bool initialize(bool debug);

		void update(double deltaTime);

		//Terminate GuiElement
		//Call this to deallocate any of GuiElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

		void editorViewports();
		void mainEditorViewport();
		void sceneHierarchyViewport();
		void inspectorViewport();
		void debuggerViewport();
		void resourceBrowserViewport();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };

		bool m_debugMode;
	};
}