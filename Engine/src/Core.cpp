#include "Core.h"

namespace Sol
{
	//Create an instance of LogElement with a unique pointer to it, other elements use nullptr until attached via attachElement
	Core::Core() : m_logElement(std::make_unique<CoreLogElement::LogElement>()), m_renderElement(nullptr) //Initializer list: initialize all elements with nullptr (except LogElement)
	{
	}
	Core::~Core()
	{
	}

	bool Core::initialize(const std::string& logfileName)
	{
		//Initialize Core
		//Initialize LogElement by calling LogElement's initialize function, passing logfileName for logger
		if (!m_logElement->initialize(logfileName))
		{
			//If LogElement can't be initialized, return error (std::cerr since no logger) and return false
			std::cerr << "Failed to initialize LogElement" << std::endl;
			return false;
		}
		m_logElement->logInfo("[Core] Successfully Initialized Sol");
		return true;
	}

	bool Core::attachElement(const std::string& elementName)
	{
		//Elements can be attached via the following mechanism:
		//Check elementName and check if Core's respective Element unique_ptr is a nullptr
		//(elementName should only be in Capitalisation case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is a nullptr
		//  Call make_unique to instantiate the Element class, giving shared_from_this to the Element's constructor
		//  Log success and return true

		if (elementName == "Render" && m_renderElement == nullptr)
		{
			m_renderElement = std::make_unique<CoreRenderElement::RenderElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		
		// ... ///
		
		//If Element can't be attached then
		m_logElement->logError(std::string("[Core] Failed To Attach ") + elementName + " Element"); //Due to left-to-right associativity and operator precendence, only implicit conversion of first literal necessary
		return false;
	}	

	//Will be automatically called by Core's terminate, but can be called manually if detaching an Element early
	bool Core::detachElement(const std::string& elementName)
	{
		//Elements can be detached via the following mechanism:
		//Check elementName and check if Core's respective Element unique_ptr is not a nullptr
		//(elementName should only be in Capitalisation case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is not a nullptr
		//  Set Core's respective Element unique_ptr to a nullptr
		//  Log success

		if (elementName == "Render" && m_renderElement != nullptr)
		{
			m_renderElement->terminate();
			m_renderElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}

		// ... ///

		//If Element can't be detached then
		m_logElement->logError(std::string("[Core] Failed To Detach ") + elementName + " Element"); //Due to left-to-right associativity and operator precendence, only implicit conversion of first literal necessary
		return false;
	}

	//void Core::update(float deltaTime)
	//{
		//Process input events
		//Update ECS
		//Update elements
		//Render current scene
		//Remember physics simulatons is fixed time, rendering and input is frame-dependent (separate function maybe?)
		//m_renderElement->update(deltaTime);
	//}
	
	void Core::terminate()
	{
		//Call terminate function(s) for attached Element(s)
		if (m_renderElement != nullptr) { detachElement("Render"); }
		if (m_logElement != nullptr)
		{
			m_logElement->logInfo("[Core] All Manually Attached Elements Successfully Detached");
			m_logElement->terminate();
			m_logElement = nullptr;
		}
	}

	// ---

	//Get functions for Elements

    //Elements can be gotten via the following mechanism:
	//Check if Core's respective Element unique_ptr is a nullptr
	//If not
	//  Log success and return a temporary raw pointer to the Core's unique_ptr (using .get())

	CoreLogElement::LogElement* Core::getLogElement() const
	{
		if (m_logElement != nullptr)
		{
			m_logElement->logInfo("[Core] Successfully Got Log Element");
			return m_logElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Log Element: nullptr found");
		return nullptr;
	}

	CoreRenderElement::RenderElement* Core::getRenderElement() const
	{
		if (m_renderElement != nullptr)
		{
			m_logElement->logInfo("[Core] Successfully Got Render Element");
			return m_renderElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Render Element: nullptr found");
		return nullptr;
	}

	// ---
}