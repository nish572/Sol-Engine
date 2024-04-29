#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <memory>
#include <string>
#include <cstring>
#include <regex>
#include <typeindex>
#include <experimental/filesystem>
#include <algorithm>
#include <cctype>

//The external libraries SDL2 and Dear ImGui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <SDL.h>

//A namespace to simplify writing the resource browser and scene browser
namespace fs = std::experimental::filesystem;

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;

namespace CoreGuiElement
{
	//Represents the Element responsible for managing the GUI operations in the Sol Engine and Editor
	class GuiElement
	{
	public:
		GuiElement(std::shared_ptr<Sol::Core> core);
		~GuiElement();

		ENGINE_API bool initialize(bool debug);

		void update(double deltaTime);

		//Call terminate to deallocate any of GUI Element's resources
		void terminate();

		//Popup window upon starting the Editor
		void startUpDialog();

		//Handles all viewports
		void editorViewports();
		//Main dockable viewport
		void mainEditorViewport();
		//Auxiliary function to clean the type name for displaying Component names in the Editor
		std::string cleanTypeName(const std::string& typeName);
		//Functions to handle all popups besides the initial dialog popup
		void handleNewScenePopup();
		void handleLoadScenePopup();
		void handleSaveScenePopup();
		void handleProjectSettingsPopup();
		//All other viewports to be handled by editorViewports, all dockable inside the main dockable viewport
		void sceneHierarchyViewport();
		void renderSceneHierarchy();
		void inspectorViewport();
		void resourceBrowserViewport();
		//Auxiliary functions to be used primarily by the resource browser to view directories and files by directory
		void displayDirectories();
		void displayFilesInDirectory();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };

		//Booleans to manage whether a popup should be visible or not
		bool m_openNewScenePopup = false;
		bool m_openLoadScenePopup = false;
		bool m_openSaveScenePopup = false;
		bool m_openProjectSettingsPopup = false;
		
		//So the running state of the scene and therefore the ECS can be managed in the Editor via buttons
		bool m_isSceneRunning = false;

		//For use by the scene hierarchy and inspector
		Entity m_selectedEntity = std::numeric_limits<std::uint32_t>::max(); //Indicate no entity is selected
		std::type_index m_selectedComponentType = typeid(void); //Default to an invalid type index

		std::string m_currentPath; //Represents the current directory path to display in the resource browser and scene browser
		std::string m_sceneLoadPath; //Represents the current loaded scene to display in the scene browser
		std::string m_selectedScene = ""; //Which scene is selected, useful primarily for saving and running the scene

		//If the application is in debug mode, i.e. the Editor should be visible not just a current loaded scene
		bool m_debugMode;
	};
}