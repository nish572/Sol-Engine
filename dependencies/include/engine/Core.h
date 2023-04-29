#pragma once

#include "EngineAPI.h"
#include <memory>
#include <iostream>

#include "render/RenderElement.h"
#include "debuglog/LogElement.h"

namespace Sol
{
	//Core represents the central manager of the Sol Engine
	//Responsible for initializing, updating, and terminating the elements
	//Responsible for allowing any Sol Applications such as the Editor to interact with the Engine (Core, elements, ECS)
	class ENGINE_API Core
	{
	public:
		//Initialize pointers to elements with new instances of elements
		Core();
		//Release resources associated with the Core instance
		~Core();

		//Attach an element in application runtime
		bool attachElement(const std::string& elementName);

		//Update Core and elements, passing the elapsed time since the last frame
		void update(float deltaTime);

		//Terminate Core, elements, and ECS, then clean up resources
		void terminate();

		//Return a pointer to the RenderElement instance managed by the Core
		CoreLogElement::LogElement* getLogElement() const;

		//Return a pointer to the RenderElement instance managed by the Core
		//CoreRenderElement::RenderElement* getRenderElement() const;

	private:
		//Smart pointers provide automatic memory management
		//When Core instance is deleted, elements also automatically deleted (and memory freed)
		//Additionally, if an exception is thrown during the initialization of Core, element objects automatically deleted
		//This ensures no memory leaks there
		//Smart pointer that holds an instance of the RenderSystem 
		std::unique_ptr<CoreLogElement::LogElement> m_logElement;
		std::unique_ptr<CoreRenderElement::RenderElement> m_renderElement;
	};
}