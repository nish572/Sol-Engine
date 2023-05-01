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
		if (elementName == "Input" && !m_eventElement)
		{
			m_eventElement = std::make_unique<CoreEventElement::EventElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Resource" && !m_resourceElement)
		{
			m_resourceElement = std::make_unique<CoreResourceElement::ResourceElement>(shared_from_this());
			m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Shader" && !m_shaderElement)
		{
			m_shaderElement = std::make_unique<CoreShaderElement::ShaderElement>(shared_from_this());
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
		if (elementName == "Input" && m_eventElement)
		{
			m_eventElement->terminate();
			m_eventElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Resource" && m_resourceElement)
		{
			m_resourceElement->terminate();
			m_resourceElement = nullptr;
			m_logElement->logInfo(std::string("[Core] Successfully Detached ") + elementName + " Element");
			return true;
		}
		if (elementName == "Shader" && m_shaderElement)
		{
			m_shaderElement = nullptr;
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
		//Initialize timing variables
		Uint64 currentTime = SDL_GetPerformanceCounter();
		Uint64 lastTime = 0;
		double deltaTime = 0.0;
		double fixedTimestep = 1.0 / 60.0; // 60 updates per second
		double accumulatedTime = 0.0;

		while (true)
		{
			lastTime = currentTime;
			currentTime = SDL_GetPerformanceCounter();
			deltaTime = static_cast<double>(currentTime - lastTime) / SDL_GetPerformanceFrequency();

			accumulatedTime += deltaTime;

			// Handle events at a fixed timestep
			while (accumulatedTime >= fixedTimestep)
			{
				std::vector<SDL_Event> events;
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					events.push_back(event);
				}
				if (m_eventElement) { m_eventElement->handleEvents(events); }
				if (!m_eventElement->isRunning()) { break; }

				accumulatedTime -= fixedTimestep;
			}

			// Render using delta timestep
			if (m_renderElement) { m_renderElement->update(deltaTime); }
			if (!m_eventElement->isRunning()) { break; }
		}
	}

	void Core::terminate()
	{
		//Call terminate function(s) for attached Element(s)
		if (m_guiElement) { detachElement("GUI"); }
		if (m_renderElement) { detachElement("Render"); }
		if (m_eventElement) { detachElement("Input"); }
		if (m_resourceElement) { detachElement("Resource"); }
		if (m_shaderElement) { detachElement("Shader"); }
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

	CoreEventElement::EventElement* Core::getEventElement() const
	{
		if (m_eventElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Input Element");
			return m_eventElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Input Element: nullptr found");
		return nullptr;
	}

	CoreResourceElement::ResourceElement* Core::getResourceElement() const
	{
		if (m_resourceElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Resource Element");
			return m_resourceElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Resource Element: nullptr found");
		return nullptr;
	}

	CoreShaderElement::ShaderElement* Core::getShaderElement() const
	{
		if (m_shaderElement)
		{
			m_logElement->logInfo("[Core] Successfully Got Shader Element");
			return m_shaderElement.get();
		}
		m_logElement->logError("[Core] Failed To Get Shader Element: nullptr found");
		return nullptr;
	}

	// ---
}