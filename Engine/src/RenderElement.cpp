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

	bool RenderElement::initialize()
	{

		/*if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			
		}*/

		return true;
	}

	void RenderElement::update(float deltaTime)
	{
	}

	void RenderElement::terminate()
	{
		//Cleanup first, so remove m_sdlWindow and m_glContext
		if (m_glContext != 0) {
			SDL_GL_DeleteContext(m_glContext);
			m_glContext = 0;
		}
		if (m_sdlWindow != nullptr) {
			SDL_DestroyWindow(m_sdlWindow);
			m_sdlWindow = nullptr;
		}
		//Remember to call SDL_Quit() only from application since other Elements may use SDL
	}

	SDL_Window* RenderElement::getWindow() const
	{
		//If m_sdlWindow isn't a nullptr
		if (m_sdlWindow != nullptr)
		{
			//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
			if (auto corePtr = m_core.lock())
			{
				//Get LogElement to log success
				corePtr->getLogElement()->logInfo("[Render] Sucessfully Got SDL Window");
			}
			//return m_sdlWindow if SDL window successfully found
			return m_sdlWindow;
		}
		//If m_sdlWindow is a nullptr
		//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
		if (auto corePtr = m_core.lock())
		{
			//Get LogElement to log error
			corePtr->getLogElement()->logError("[Render] Cannot Get SDL Window: nullptr found");
		}
		//Return a nullptr if SDL window not found successfully
		return nullptr;
	}

	SDL_GLContext RenderElement::getGLContext() const
	{
		//If m_glContext isn't a nullptr
		if (m_glContext != 0)
		{
			//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
			if (auto corePtr = m_core.lock())
			{
				//Get LogElement to log success
				corePtr->getLogElement()->logInfo("[Render] Successfully Got OpenGL Context");
			}
			//Return m_sdlWindow if OpenGL context successfully found
			return m_glContext;
		}
		//If m_glContext is a nullptr
		//Temporarily upgrade weak pointer to shared pointer, ownership automatically released once out of scope due to reference count
		if (auto corePtr = m_core.lock())
		{
			//Get LogElement to log error
			corePtr->getLogElement()->logError("[Render] Cannot Get OpenGL Context: nullptr found");
		}
		//Return a nullptr if OpenGL context not found successfully
		return nullptr;
	}
}