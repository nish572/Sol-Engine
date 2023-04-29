#include "render/RenderElement.h"

namespace CoreRenderElement
{
	RenderElement::RenderElement() : m_window(nullptr), m_glContext(0), loggerAttached(false)
	{
	}
	RenderElement::~RenderElement()
	{
	}

	bool RenderElement::initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			
		}

		return true;
	}

	void RenderElement::update(float deltaTime)
	{
	}

	void RenderElement::terminate()
	{
	}

	SDL_Window* RenderElement::getWindow() const
	{
		return m_window;
	}

	SDL_GLContext RenderElement::getGLContext() const
	{
		return m_glContext;
	}
}