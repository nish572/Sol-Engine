#pragma once

#include "EngineAPI.h"

#pragma warning(push)
#pragma warning(disable: 4018)

//C++ libraries
#include <memory>
#include <string>

//External libraries GLAD and SDL2
#include <glad.h>
#include <SDL.h>
#pragma warning(pop)

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreRenderElement
{
	//Represents the Element responsible for managing the rendering operations in the Sol Engine
	//This includes SDL window management and OpenGL context management
	class RenderElement
	{
	public:
		RenderElement(std::shared_ptr<Sol::Core> core);
		~RenderElement();

		//Initialize RenderElement by creating an SDL window and OpenGL context
		ENGINE_API bool initialize(const std::string& windowName, float width, float height, SDL_WindowFlags window_flags, int vsync);

		//Clear the screen, useful as otherwise OpenGL will render the next frame over the last
		void clearScreen();
		
		//Swap frame buffers, useful as OpenGL renders the next frame in the background while displaying the current frame, swap to make this next frame visible
		void swapBuffers();

		//Set window size
		void setWindowSize(float width, float height);

		//Terminate Render Element by destroying the SDL window and the OpenGL context
		void terminate();

		//Return a pointer to the SDL window instance managed by the Render Element
		SDL_Window* getWindow() const;
		//Return an OpenGL context handle managed by the Render Element
		SDL_GLContext getGLContext() const;

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		//A pointer to the SDL window instanced managed by the Render Element
		SDL_Window* m_sdlWindow;
		//An OpenGL context handle managed by the Render Element
		SDL_GLContext m_glContext;
	};
}