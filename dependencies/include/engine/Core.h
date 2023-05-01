#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>
#include <string>

#include "render/RenderElement.h"
#include "debuglog/LogElement.h"

namespace Sol
{
	//Core represents the central manager of the Sol Engine
	//Responsible for initializing, updating, and terminating the Elements
	//Responsible for allowing any Sol Applications such as the Editor to interact with the Engine (Core, Elements, ECS)
	class Core : public std::enable_shared_from_this<Core>
	{
	public:
		//Initialize pointers to Elements with new instances of Elements
		ENGINE_API Core();
		//Release resources associated with the Core instance
		ENGINE_API ~Core();

		//Attach an Element in application runtime
		//To make an Element usable by the Core, ammend the attachElement code to include the Element
		//elementName should only be in Capitalisation case e.g. 'Render', and should not include 'Element'
		ENGINE_API bool attachElement(const std::string& elementName);

		//Detach an Element
		//Preferably, do so ONLY from Core's terminate function as this checks to see if each Element is not a nullptr
		// i.e. if the Element is still attached, and if so, call Element's terminate functon
		// which automatically calls detachElement once Element's termination complete
		//However, if detachElement call desired prior to terminating Core, ensure the Element's terminate function has been called first
		ENGINE_API bool detachElement(const std::string& elementName);

		//Initialize Core (namely ensure logger initialised)
		//Return true/false to be handled by the application
		ENGINE_API bool initialize(const std::string& logfileName);

		//Update Core and Elements, passing the elapsed time since the last frame
		//void update(float deltaTime);

		//Call terminate functions for all Elements, first checking they're not nullptrs
		ENGINE_API void terminate();

		// ---

		//Get functions for Elements

		//Return a pointer to the RenderElement instance managed by the Core
		ENGINE_API CoreLogElement::LogElement* getLogElement() const;

		//Return a pointer to the RenderElement instance managed by the Core
		ENGINE_API CoreRenderElement::RenderElement* getRenderElement() const;

		// ... //

		// ---

	private:
		//Smart pointers provide automatic memory management
		//When Core instance is deleted, Elements also automatically deleted (and memory freed)
		//Additionally, if an exception is thrown during the initialization of Core, Element objects automatically deleted
		//This ensures no memory leaks there
		
		//Smart pointer(s) for an instance of the each Element
		// --- 
		std::unique_ptr<CoreLogElement::LogElement> m_logElement;
		std::unique_ptr<CoreRenderElement::RenderElement> m_renderElement;
		// ... //
		// ---
	};
}