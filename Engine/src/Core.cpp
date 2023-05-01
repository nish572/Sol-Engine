#include "Core.h"

namespace Sol
{
	//Create an instance of LogElement with a unique pointer to it, other elements use nullptr until attached via attachElement
	Core::Core() : m_logElement(std::make_unique<CoreLogElement::LogElement>())
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
		//(elementName should only be in Capitalization case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is a nullptr
		//  Call make_unique to instantiate the Element class, giving shared_from_this to the Element's constructor
		//  Log success and return true

		if (elementName == "Render" && !m_renderElement)
		{
			m_renderElement = std::make_unique<CoreRenderElement::RenderElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		if (elementName == "GUI" && !m_guiElement)
		{
			m_guiElement = std::make_unique<CoreGUIElement::GUIElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Input" && !m_inputElement)
		{
			m_inputElement = std::make_unique<CoreInputElement::InputElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		
		//If Element can't be attached then
		m_logElement->logError(std::string("[Core] Failed To Attach ") + elementName + " Element"); //Due to left-to-right associativity and operator precedence, only implicit conversion of first literal necessary
		return false;
	}	

	//Will be automatically called by Core's terminate, but can be called manually if detaching an Element early
	bool Core::detachElement(const std::string& elementName)
	{
		//Elements can be detached via the following mechanism:
		//Check elementName and check if Core's respective Element unique_ptr is not a nullptr
		//(elementName should only be in Capitalization case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is not a nullptr
		//  Set Core's respective Element unique_ptr to a nullptr
		//  Log success

		if (elementName == "Render" && m_renderElement)
		{
			m_renderElement->terminate();
			m_renderElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}
		if (elementName == "GUI" && m_guiElement)
		{
			m_guiElement->terminate();
			m_guiElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Input" && m_inputElement)
		{
			m_inputElement->terminate();
			m_inputElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}

		// ... ///

		//If Element can't be detached then
		m_logElement->logError(std::string("[Core] Failed To Detach ") + elementName + " Element"); //Due to left-to-right associativity and operator precedence, only implicit conversion of first literal necessary
		return false;
	}

	void Core::run()
	{
		//Call update function(s) for attached Element(s)
		std::vector<SDL_Event> events;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			events.push_back(event);
		}
		if (m_renderElement) { m_renderElement->update(); }
		if (m_inputElement) { m_inputElement->handleEvents(events); }
	}
	
	void Core::terminate()
	{
		//Call terminate function(s) for attached Element(s)
		if (m_guiElement) { detachElement("GUI"); }
		if (m_renderElement) { detachElement("Render"); }
		if (m_logElement)
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
		if (m_logElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Log Element");
			return m_logElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Log Element: nullptr found");
		return nullptr;
	}

	CoreRenderElement::RenderElement* Core::getRenderElement() const
	{
		if (m_renderElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Render Element");
			return m_renderElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Render Element: nullptr found");
		return nullptr;
	}

	CoreGUIElement::GUIElement* Core::getGUIElement() const
	{
		if (m_guiElement)
		{
			m_logElement->logInfo("[Core] Successfully Got GUI Element");
			return m_guiElement.get();
		}
		m_logElement->logError("[Core] Failed To Get GUI Element: nullptr found");
		return nullptr;
	}

	CoreInputElement::InputElement* Core::getInputElement() const
	{
		if (m_inputElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Input Element");
			return m_inputElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Input Element: nullptr found");
		return nullptr;
	}

	// ---
}