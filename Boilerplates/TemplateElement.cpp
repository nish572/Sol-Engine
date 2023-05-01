//MODIFY: Replace 'Template' with desired name for Element including in file name
//Remember to modify Core.h and Core.cpp accordingly
 
//Include the TemplateElement header file e.g. #include "render/RenderElement.h"
#include "template/TemplateElement.h"

//#Include Core.h
//Core.h's full definiton required
#include "Core.h"

namespace CoreTemplateElement
{
	//TemplateElement has intializer list
	//TemplateElement MUST have at least m_core private member
	//If TemplateElement has private member(s) for pointer(s) to object(s) managed by TemplateElement
	//then include in intializer list here with nullptr as value(s)
	//e.g. RenderElement's initializer list is: m_core(core), m_window(nullptr), m_glContext(0)
	TemplateElement::TemplateElement(std::shared_ptr<Sol::Core> core) : m_core(core) //extend initializer list if necessary as above
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

	//Get function(s) for pointer(s) to object(s) managed by TemplateElement
	// ... //
}