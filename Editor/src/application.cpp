//------- Sol Editor -------
//A Game Editor Utilizing
//The Sol Core
//--------------------------
#include "Core.h"

int main(int argc, char* args[]) {
	//Core instance (make_shared allows Elements to have a weak_ptr to Core, important for Element's to access each other if desired)
	auto appCore = std::make_shared<Sol::Core>();

	//Attach and initialize any Element(s) by name
	//Setup render before GUI
	//Setup physics/render/event before ECS
	//Attach elements before calling initialize functions
	//Initialize log element (if using) before attaching and initializing other elements to ensure can log from them to the specified txt file

	appCore->attachElement("Log");
	appCore->getLogElement()->initialize("Sol-Editor.txt");

	appCore->attachElement("Shader");
	appCore->attachElement("Resource");
	appCore->attachElement("Render");
	appCore->attachElement("Gui");
	appCore->attachElement("Physics");
	appCore->attachElement("Event");
	appCore->attachElement("Ecs");
	appCore->attachElement("Scene");

	appCore->getShaderElement()->initialize();
	appCore->getResourceElement()->initialize();
	//Setting the window name to "Sol Editor", width to 1920, height to 1080,
	//window flags to (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), and vsync to 0 (disabled)
	appCore->getRenderElement()->initialize("Sol Engine", 800, 600, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0); //Render element does not depend upon any other element
	appCore->getGuiElement()->initialize(true); //Gui element depends upon render element, render element must be initialized first
	appCore->getPhysicsElement()->initialize();
	appCore->getEventElement()->initialize();
	//Setting each system to true or false depending on whether or not they need to be used (for full ECS functionality, true for each required)
	appCore->getEcsElement()->initialize(true, true, true);
	appCore->getSceneElement()->initialize();	

	//Runtime loop
	bool appRunning = true;

	while (appRunning)
	{
		//Run Core, passing event (so EventElement can process events)
		appCore->run();
		//Keep app running unless SDL_QUIT event received or SDL_WINDOWEVENT_CLOSE event received for appCore's window
		appRunning = appCore->getEventElement()->isRunning();
	}

	//Good practice to call Core's terminate function in case any Elements are still attached
	appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}