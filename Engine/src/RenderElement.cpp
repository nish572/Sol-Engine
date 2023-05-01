#include "render/RenderElement.h"

#include "Core.h"

namespace CoreRenderElement
{
	RenderElement::RenderElement(std::shared_ptr<Sol::Core> core) : m_core(core), m_sdlWindow(nullptr), m_glContext(0) //Initialize m_sdlWindow and m_glContext to nullptrs
	{
	}
	RenderElement::~RenderElement()
	{
	}

	bool RenderElement::initialize(const std::string& windowName, int width, int height, SDL_WindowFlags window_flags, int vsync)
	{
		//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
		//Check if logger available and if so then continue, otherwise return false
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (!corePtr->getLogElement())
			{
				std::cerr <<  "Failed to initialize RenderElement: LogElement is a nullptr" << std::endl;
				return false;
			}
		}

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logError(std::string("[Render] Failed To Initialize SDL") + SDL_GetError());
			}
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
				if (corePtr)
				{
					corePtr->getLogElement()->logError(std::string("[Render] Failed To Create SDL Window") + SDL_GetError());
				}
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
				if (corePtr)
				{
					corePtr->getLogElement()->logError(std::string("[Render] Failed To Create OpenGL Context") + SDL_GetError());
				}
				return false;
			}
		}
		//If SDL initialized successfully, SDL window created successfully, and OpenGL context created successfully
		//then log success and return true
		if (corePtr)
		{
			corePtr->getLogElement()->logInfo("[Render] Successfully Initialized");
		}
		return true;
	}

	void RenderElement::update()
	{
		glClearColor(0.7f, 0.3f, 1.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getGUIElement())
			{
				corePtr->getGUIElement()->update();
			}
		}
		SDL_GL_SwapWindow(m_sdlWindow);
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
		//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
		auto corePtr = m_core.lock();
		//If m_sdlWindow isn't a nullptr
		if (m_sdlWindow)
		{
			if (corePtr)
			{
				//Get LogElement to log success
				corePtr->getLogElement()->logInfo("[Render] Successfully Got SDL Window");
			}
			//return m_sdlWindow if SDL window successfully found
			return m_sdlWindow;
		}
		//If m_sdlWindow is a nullptr
		if (!m_sdlWindow)
		{
			//Get LogElement to log error
			corePtr->getLogElement()->logError("[Render] Failed To Get SDL Window: nullptr found");
		}
		//Return a nullptr if SDL window not found successfully
		return nullptr;
	}

	SDL_GLContext RenderElement::getGLContext() const
	{
		//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
		auto corePtr = m_core.lock();
		//If m_glContext isn't a nullptr
		if (m_glContext)
		{
			if (corePtr)
			{
				//Get LogElement to log success
				corePtr->getLogElement()->logInfo("[Render] Successfully Got OpenGL Context");
			}
			//Return m_sdlWindow if OpenGL context successfully found
			return m_glContext;
		}
		//If m_glContext is a nullptr
		if (!m_glContext)
		{
			//Get LogElement to log error
			corePtr->getLogElement()->logError("[Render] Failed To Get OpenGL Context: nullptr found");
		}
		//Return a nullptr if OpenGL context not found successfully
		return nullptr;
	}
}