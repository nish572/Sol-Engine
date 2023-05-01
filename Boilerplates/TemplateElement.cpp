//MODIFY: Replace 'Template' with desired name for Element including in file name
//Remember to modify Core.h and Core.cpp accordingly
 
//Include the TemplateElement header file e.g. #include "render/RenderElement.h"
#include "template/TemplateElement.h"

//#Include Core.h
//Core.h's full definition required
#include "Core.h"

namespace CoreTemplateElement
{
	//TemplateElement has initializer list for any managed resources that require initializing
	//All Elements MUST have at least m_core private member
	TemplateElement::TemplateElement(std::shared_ptr<Sol::Core> core) : m_core(core) //Extend initializer list if necessary
	{
	}
	TemplateElement::~TemplateElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool TemplateElement::initialize()
	{
		// ... //
	}

	//Define any custom functionality declared in header
	// ... //

	void TemplateElement::terminate()
	{
		//Any TemplateElement cleanup
		// ... //
	}

	//Get function(s) for (pointer(s) to) object(s) managed by TemplateElement
	// ... //
}