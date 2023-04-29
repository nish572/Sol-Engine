#include "Core.h"

namespace Sol
{
	Core::Core() :  m_logElement(nullptr), m_renderElement(nullptr)
	{
	}
	Core::~Core()
	{
	}

	bool Core::attachElement(const std::string& elementName)
	{
		if (elementName == "Log")
		{
			std::cout << "Initializing LogElement..." << std::endl;
			m_logElement = std::make_unique<CoreLogElement::LogElement>();
			m_logElement->initialize();
			return true;
		}
		if (elementName == "Render")
		{
			m_renderElement = std::make_unique<CoreRenderElement::RenderElement>();
			m_renderElement->initialize();
			return true;
		}
		return false;
	}	

	void Core::update(float deltaTime)
	{
		//Process input events
		//Update ECS
		//Update elements
		//Render current scene
		//Remember physics simulatons is fixed time, rendering and input is frame-dependent (separate function maybe?)
		//m_renderElement->update(deltaTime);
	}
	
	void Core::terminate()
	{
		//Release any resources loaded during engine lifetime (delete res and/or free res allocated mem)
		//Call termination methods for each custom element, each method performing necessary cleanup
		//Delete element instances
		//Release ECS resource
		//Call termination functions for any external libraries
		m_logElement->terminate();
	}

	CoreLogElement::LogElement* Core::getLogElement() const
	{
		//.get returns a raw pointer to the LogElement instance
		return m_logElement.get();
	}

	//CoreRenderElement::RenderElement* Core::getRenderElement() const
	//{
	//	//.get returns a raw pointer to the RenderElement instance
	//	return m_renderElement.get();
	//}
}