#include "Core.h"

namespace Sol
{
	//Create an instance of LogElement with a unique pointer to it, other elements use nullptr until attached via attachElement
	Core::Core()
	{
	}
	Core::~Core()
	{
	}

	bool Core::initialize()
	{
		//Any initial checks for Core initialization
		return true;
	}

	bool Core::attachElement(const std::string& elementName)
	{
		//Elements can be attached via the following mechanism:
		//Check elementName and check if Core's respective Element unique_ptr is a nullptr
		//(elementName should only be in Capitalization case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is a nullptr
		// Call make_unique to instantiate the Element class, giving shared_from_this to the Element's constructor
		// Log success and return true

		if (elementName == "Log" && !m_logElement)
		{
			m_logElement = std::make_unique<CoreLogElement::LogElement>(shared_from_this());
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Shader" && !m_shaderElement)
		{
			m_shaderElement = std::make_unique<CoreShaderElement::ShaderElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Render" && !m_renderElement)
		{
			m_renderElement = std::make_unique<CoreRenderElement::RenderElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Gui" && !m_guiElement)
		{
			m_guiElement = std::make_unique<CoreGuiElement::GuiElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Event" && !m_eventElement)
		{
			m_eventElement = std::make_unique<CoreEventElement::EventElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Resource" && !m_resourceElement)
		{
			m_resourceElement = std::make_unique<CoreResourceElement::ResourceElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Ecs" && !m_ecsElement)
		{
			m_ecsElement = std::make_shared<CoreEcsElement::EcsElement>(shared_from_this());
			if (m_logElement)
			{
				m_logElement->logInfo(std::string("[Core] Successfully Attached ") + elementName + " Element");
				return true;
			}
			std::cout << "[Core] Successfully Attached " << elementName << " Element" << std::endl;
			return true;
		}
		
		//If Element can't be attached then
		if (m_logElement)
		{
			m_logElement->logError(std::string("[Core] Failed To Attach ") + elementName + " Element"); //Due to left-to-right associativity and operator precedence, only implicit conversion of first literal necessary
			return false;
		}
		std::cerr << "[Core] Failed To Attach " << elementName << " Element" << std::endl;
		return false;
	}	

	//Will be automatically called by Core's terminate, but can be called manually if detaching an Element early
	bool Core::detachElement(const std::string& elementName)
	{
		//Elements can be detached via the following mechanism:
		//Check elementName and check if Core's respective Element unique_ptr is not a nullptr
		//(elementName should only be in Capitalization case e.g. 'Render', and should not include 'Element')
		//If Core's respective Element unique_ptr is not a nullptr
		//Set Core's respective Element unique_ptr to a nullptr
		//Log success

		if (elementName == "Log" && m_logElement)
		{
			m_logElement->terminate();
			m_logElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Render" && m_renderElement)
		{
			m_renderElement->terminate();
			m_renderElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Gui" && m_guiElement)
		{
			m_guiElement->terminate();
			m_guiElement = nullptr;
			std::cout  << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Event" && m_eventElement)
		{
			m_eventElement->terminate();
			m_eventElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Resource" && m_resourceElement)
		{
			m_resourceElement->terminate();
			m_resourceElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Shader" && m_shaderElement)
		{
			m_shaderElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}
		if (elementName == "Ecs" && m_ecsElement)
		{
			//m_ecsElement->terminate();
			m_ecsElement = nullptr;
			std::cout << "[Core] Successfully Detached " << elementName << " Element" << std::endl;
			return true;
		}

		//... ///

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
		double fixedTimestep = 1.0 / 60.0; //60 updates per second
		double accumulatedTime = 0.0;

		while (true)
		{
			//Update timing variables
			lastTime = currentTime;
			currentTime = SDL_GetPerformanceCounter();
			deltaTime = static_cast<double>(currentTime - lastTime) / SDL_GetPerformanceFrequency();
			deltaTime = std::min(deltaTime, 0.25); //Prevent deltaTime from being too large, essentially the 'spiral of death' safeguard.
			accumulatedTime += deltaTime;

			//Handle events at a fixed timestep
			while (accumulatedTime >= fixedTimestep)
			{
				std::vector<SDL_Event> events;
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					events.push_back(event);
				}
				if (m_eventElement) { m_eventElement->handleEvents(events); }
				if (m_ecsElement) { m_ecsElement->fixedUpdate(fixedTimestep); }
				if (!m_eventElement->isRunning()) { break; }

				accumulatedTime -= fixedTimestep;
			}

			//Render using delta timestep
			if (m_renderElement) { m_renderElement->clearScreen(); }
			if (m_ecsElement) { m_ecsElement->update(deltaTime); }
			if (m_guiElement) { m_guiElement->update(deltaTime); }
			if (m_renderElement) { m_renderElement->swapBuffers(); }
			if (!m_eventElement->isRunning()) { break; }
		}
	}

	void Core::terminate()
	{
		//Call terminate function(s) for attached Element(s)
		if (m_guiElement) { detachElement("Gui"); }
		if (m_renderElement) { detachElement("Render"); }
		if (m_eventElement) { detachElement("Event"); }
		if (m_resourceElement) { detachElement("Resource"); }
		if (m_shaderElement) { detachElement("Shader"); }
		if (m_ecsElement) { detachElement("Ecs"); }
		if (m_logElement) { detachElement("Log"); }
		std::cout << "[Core] Successfully Terminated" << std::endl;
	}

	//---

	//Get functions for Elements

    //Elements can be gotten via the following mechanism:
	//Check if Core's respective Element unique_ptr is a nullptr
	//If not, return a temporary raw pointer to the Core's unique_ptr (using .get())
	//Otherwise, log error and return nullptr

	CoreLogElement::LogElement* Core::getLogElement() const
	{
		if (m_logElement)
		{
			return m_logElement.get();
		}
		std::cerr << "[Core] Failed To Get Log Element: nullptr found" << std::endl;
		return nullptr;
	}

	CoreRenderElement::RenderElement* Core::getRenderElement() const
	{
		if (m_renderElement)
		{
			return m_renderElement.get();
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get Render Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get Render Element: nullptr found" << std::endl;
		return nullptr;
	}

	CoreGuiElement::GuiElement* Core::getGuiElement() const
	{
		if (m_guiElement)
		{
			return m_guiElement.get();
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get Gui Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get Gui Element: nullptr found" << std::endl;
		return nullptr;
	}

	CoreEventElement::EventElement* Core::getEventElement() const
	{
		if (m_eventElement)
		{
			return m_eventElement.get();
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get Event Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get Event Element: nullptr found" << std::endl;
		return nullptr;
	}

	CoreResourceElement::ResourceElement* Core::getResourceElement() const
	{
		if (m_resourceElement)
		{
			return m_resourceElement.get();
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get Resource Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get Resource Element: nullptr found" << std::endl;
		return nullptr;
	}

	CoreShaderElement::ShaderElement* Core::getShaderElement() const
	{
		if (m_shaderElement)
		{
			return m_shaderElement.get();
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get Shader Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get Shader Element: nullptr found" << std::endl;
		return nullptr;
	}

	std::shared_ptr<CoreEcsElement::EcsElement> Core::getEcsElement() const
	{
		if (m_ecsElement)
		{
			return m_ecsElement;
		}
		if (m_logElement)
		{
			m_logElement->logError("[Core] Failed To Get ECS Element: nullptr found");
			return nullptr;
		}
		std::cerr << "[Core] Failed To Get ECS Element: nullptr found" << std::endl;
		return nullptr;
	}

	//---
}