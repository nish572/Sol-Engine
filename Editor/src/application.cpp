//------- Sol Editor -------
//An Game Editor Utilizing
//The Sol Core
//--------------------------
#include "Core.h"

int main(int argc, char* args[]) {

	//Core instance (make_shared allows Elements to have a weak_ptr to Core, important for Element's to access each other if desired)
	auto appCore = std::make_shared<Sol::Core>();

	//Attach any Element(s) by name
	appCore->attachElement("Log");
	appCore->getLogElement()->initialize("Sol-Editor.txt");

	appCore->attachElement("Render");
	appCore->attachElement("Gui");
	appCore->attachElement("Event");
	appCore->attachElement("Resource");
	appCore->attachElement("Shader");
	appCore->attachElement("Ecs");

	//Initialize any Element(s) by name
	//Here I'm setting the window name to "Sol Editor", width to 1920, height to 1080,
	
	//window flags to (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), and vsync to 0 (disabled)
	appCore->getRenderElement()->initialize("Sol Editor", 1080, 720, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0); //Render element does not depend upon any other element
	appCore->getGuiElement()->initialize(); //Gui element depends upon render element, render element must be initialized first
	appCore->getEventElement()->initialize(); //Event element depends upon render element, render element must be initialized first
	appCore->getResourceElement()->initialize(); //Resource element does not depend upon any other element
	appCore->getShaderElement()->initialize(); //Shader element does not depend upon any other element
	appCore->getEcsElement()->initialize(); //Ecs element depends on render element, event element, resource element, and possibly shader element

	//Runtime loop
	bool appRunning = true;
	while (appRunning)
	{
		//Run Core, passing event (so EventElement can process events)
		appCore->run();

		//Keep app running unless SDL_QUIT event received or SDL_WINDOWEVENT_CLOSE event received for appCore's window
		appRunning = appCore->getEventElement()->isRunning();
	}

	//Good practice to
	//Call Core's terminate function in case any Elements are still attached
	//appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}