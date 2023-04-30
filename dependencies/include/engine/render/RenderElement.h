#pragma once

#include "EngineAPI.h"

#include <memory>

#include <glad.h>
#include <SDL.h>

//Forward declation of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreRenderElement
{
	//Represents the element responsible for managing the rendering operations in the Sol Engine
	//This includes SDL window management and OpenGL context management
	class RenderElement
	{
	public:
		//Initialize pointers to SDL window and OpenGL context
		RenderElement(std::shared_ptr<Sol::Core> core);
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
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//A pointer to the SDL window instanced managed by the RenderElement
		SDL_Window* m_sdlWindow;
		//An OpenGL context handle managed by the RenderElement
		SDL_GLContext m_glContext;
	};
}