#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>

#include <glad.h>
#include <SDL.h>

namespace CoreRenderElement
{
	//Represents the element responsible for managing the rendering operations in the Sol Engine
	//This includes SDL window management and OpenGL context management
	class RenderElement
	{
	public:
		//Initialize pointers to SDL window and OpenGL context
		RenderElement();
		//Release resources associated with the RenderElement instance
		~RenderElement();

		//Initialize RenderElement by creating an SDL window and OpenGL context
		bool initialize();

		//Update RenderElement, passing the elapsed time since the last frame
		void update(float deltaTime);

		//Terminate RenderElement by destroying the SDL window and the OpenGL context
		void terminate();

		//Return a pointer to the SDL window instance managed by the RenderElement
		ENGINE_API SDL_Window* getWindow() const;
		//Return an OpenGL context handle managed by the RenderElement
		ENGINE_API SDL_GLContext getGLContext() const;

	private:
		//A pointer to the SDL window instanced managed by the RenderElement
		SDL_Window* m_window;
		//An OpenGL context handle managed by the RenderElement
		SDL_GLContext m_glContext;
		bool loggerAttached;
	};
}