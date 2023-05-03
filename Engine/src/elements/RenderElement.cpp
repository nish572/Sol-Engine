#include "render/RenderElement.h"

#include "Core.h"

namespace CoreRenderElement
{
	RenderElement::RenderElement(std::shared_ptr<Sol::Core> core) : m_core(core), m_sdlWindow(nullptr), m_glContext(0)
	{
	}
	RenderElement::~RenderElement()
	{
	}

	bool RenderElement::initialize(const std::string& windowName, int width, int height, SDL_WindowFlags window_flags, int vsync)
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			if (m_logElementAttached)
			{
				if (corePtr)
				{
					corePtr->getLogElement()->logError(std::string("[Render] Failed To Initialize SDL") + SDL_GetError());
				}
				return false;
			}
			std::cerr << "[Render] Failed To Initialize SDL" << std::endl;
			return false;
		}
		else
		{
			//Set OpenGL version
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); //Set version to 3
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); //Specifically set to X.3
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //Chose glad-core when getting glad rather than compatibility for reliability
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //Believe defaults to 1 anyways, but explicitly writing for readability and assurance
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); //24 bit color allows full color range, even though 8 bits fine for human eye, more bits=higher quality
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); //8 bit stencil buffer, 8 bits fine for stencil buffer, more bits=higher quality

			//Create SDL window
			m_sdlWindow = SDL_CreateWindow(windowName.c_str(), //.c_str() converts windowName to const char* as that is SDL param type
				SDL_WINDOWPOS_CENTERED, //Center window horizontally
				SDL_WINDOWPOS_CENTERED, //Center window vertically
				width, height, //Set window size
				window_flags); //Set window flags

			if (!m_sdlWindow)
			{
				if (m_logElementAttached)
				{
					if (corePtr)
					{
						corePtr->getLogElement()->logError(std::string("[Render] Failed To Create SDL Window") + SDL_GetError());
					}
					return false;
				}
				std::cerr << "[Render] Failed To Create SDL Window" << std::endl;
				return false;
			}

			//Create OpenGL context
			m_glContext = SDL_GL_CreateContext(m_sdlWindow);
			//Associate OpenGL context with SDL window
			SDL_GL_MakeCurrent(m_sdlWindow, m_glContext);
			//Set vsync (0=off, 1=on, -1=adaptive vsync)
			SDL_GL_SetSwapInterval(vsync);

			//Setup function pointers
			if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
				if (m_logElementAttached)
				{
					if (corePtr)
					{
						corePtr->getLogElement()->logError(std::string("[Render] Failed To Create OpenGL Context") + SDL_GetError());
					}
					return false;
				}
				std::cerr << "[Render] Failed To Create OpenGL Context" << std::endl;
				return false;
			}
		}
		//If SDL initialized successfully, SDL window created successfully, and OpenGL context created successfully
		//then log success and return true
		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Render] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Render] Successfully Initialized" << std::endl;
		return true;
	}

	void RenderElement::update(double deltaTime)
	{
		glClearColor(0.7f, 0.3f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			auto guiElement = corePtr->getGuiElement();
			if (guiElement)
			{
				guiElement->update(deltaTime);
			}
		}
		SDL_GL_SwapWindow(m_sdlWindow);
	}

	void RenderElement::drawTexture(const std::string& texturePath, float x, float y, float width, float height)
	{
		// Load the texture using ResourceElement
		auto corePtr = m_core.lock();
		if (!corePtr)
		{
			std::cerr << "[Render] Failed To Draw Texture: Cannot Access Core" << std::endl;
			return;
		}

		auto resourceElement = corePtr->getResourceElement();
		if (!resourceElement)
		{
			if (m_logElementAttached)
			{
				corePtr->getLogElement()->logError("[Render] Failed To Draw Texture: Cannot Access Resource Element");
				return;
			}
			std::cerr << "[Render] Failed To Draw Texture: Cannot Access Resource Element" << std::endl;
			return;
		}

		auto textureResource = resourceElement->loadTextureResource(texturePath);
		if (!textureResource)
		{
			if (m_logElementAttached)
			{
				corePtr->getLogElement()->logError("[Render] Failed To Draw Texture: Cannot Load Texture Resource");
				return;
			}
			std::cerr << "[Render] Failed To Draw Texture: Cannot Load Texture Resource" << std::endl;
			return;
		}

		GLuint textureID = textureResource->textureID;

		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set up the vertex data for the sprite
		float vertices[] = {
			// positions          // texture coords
			x,          y,          0.0f, 0.0f, // top left
			x + width,  y,          1.0f, 0.0f, // top right
			x + width,  y + height, 1.0f, 1.0f, // bottom right
			x,          y + height, 0.0f, 1.0f  // bottom left
		};

		// Set up the vertex buffer object and vertex array object
		GLuint VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Draw the sprite
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// Clean up
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	void RenderElement::terminate()
	{
		//Cleanup first, so remove m_sdlWindow and m_glContext
		if (m_glContext) {
			SDL_GL_DeleteContext(m_glContext);
			m_glContext = 0;
		}
		if (m_sdlWindow) {
			SDL_DestroyWindow(m_sdlWindow);
			m_sdlWindow = nullptr;
		}
		//Safe to call here as RenderElement terminated last
		SDL_Quit();
	}

	SDL_Window* RenderElement::getWindow() const
	{
		// If SDL window is found, return m_sdlWindow
		if (m_sdlWindow)
		{
			return m_sdlWindow;
		}

		// If no SDL window found, log error and return nullptr
		auto corePtr = m_core.lock();
		if (m_logElementAttached)
		{
			// Get LogElement to log error
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Render] Failed To Get SDL Window: nullptr found");
			}
		}
		std::cerr << "[Render] Failed To Get SDL Window: nullptr found" << std::endl;
		return nullptr;
	}


	SDL_GLContext RenderElement::getGLContext() const
	{
		// If m_glContext is found, return m_glContext
		if (m_glContext)
		{
			return m_glContext;
		}

		// If no OpenGL context found, log error and return nullptr
		auto corePtr = m_core.lock();
		if (m_logElementAttached)
		{
			// Get LogElement to log error
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Render] Failed To Get OpenGL Context: nullptr found");
			}
		}
		std::cerr << "[Render] Failed To Get OpenGL Context: nullptr found" << std::endl;
		return nullptr;
	}
}