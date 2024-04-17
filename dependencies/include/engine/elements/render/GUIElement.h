#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>
#include <regex>
#include <typeindex>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <SDL.h>

using Entity = std::uint32_t;

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
		std::string cleanTypeName(const std::string& typeName);
		void handleProjectSettingsPopup();
		void sceneHierarchyViewport();
		void renderSceneHierarchy();
		void inspectorViewport();
		void debuggerViewport();
		void resourceBrowserViewport();

		std::vector<std::pair<std::string, SDL_Keycode>> getKeyCodeList();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };

		bool m_openProjectSettingsPopup = false;

		bool m_isSceneRunning = false;

		Entity m_selectedEntity = std::numeric_limits<std::uint32_t>::max(); //Indicate no entity is selected
		std::type_index m_selectedComponentType = typeid(void); //Default to an invalid type index

		bool m_debugMode;
	};
}