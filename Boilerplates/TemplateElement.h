//MODIFY: Replace 'Template' with desired name for Element including in file name
//Remember to modify Core.h and Core.cpp accordingly
 
// ------- Necessary in every Element header -------
//Prevent header file from being included multiple times
#pragma once
//If a function/class needs to be used directly in the application
//Include EngineAPI.h and declare function/class with ENGINE_API macro
//WARNING: Use carefully to ensure valid encapsulation
#include "EngineAPI.h"
//Include <memory> to manage the link between Core and this Element
#include <memory>

//Include any of this Element's specific required includes, e.g. SDL or linmath
// ... //

//Forward declation of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

// -------------------------------------------------

// ------- Modify Template Element -------
namespace CoreTemplateElement
{
	//State functionality of Element in a comment here
	class TemplateElement
	{
	public:
		//Construct TemplateElement and pass shared_ptr to Core
		//This is because TemplateElement's m_core is a weak_ptr, and requires a shared_ptr to be passed
		TemplateElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the TemplateElement instance
		~TemplateElement();

		//Initialize TemplateElement
		//Call this after calling TemplateElement's attachElement
		bool initialize();

		//Any custom functionality
		// ... //

		//Terminate TemplateElement
		//Call this to deallocate any of TemplateElement's resources
		//Call this when ammending Core's detachElement function and inside Core's terminate function
		void terminate();

		//If required, ensure object(s) managed by TemplateElement (e.g. RenderElement manages SDL Window)
		//Have get function(s) for pointer(s) to object(s) managed by TemplateElement
		//where pointer(s) is (are) private member(s) of TemplateElement
		//Remember to check if the private member is nullptr, and if it is log error and return nullptr
		//If the private member is valid, log success and return private member
		// ... //

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Pointer(s) to TemplateElement's object(s)
		//e.g. in RenderElement
		//SDL_Window* m_sdlWindow;
		// ... //
	};
}

//To access one Element from another Element (or any of another Element's member(s)):
// ---
//if (auto corePtr = m_core.lock())
//{
//	Access other Elements or members using corePtr
//  e.g. corePtr->getLogElement->logInfo("Write Info Msg Here")
//}
// ---