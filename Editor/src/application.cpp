// ------- Sol Editor -------
// An Game Editor Utilising
// The Sol Core
// --------------------------
#include "Core.h"

int main(int argc, char* args[]) {
	
	//Core instance (make_shared allows Element's to have a weak_ptr to Core, important for Element's to access each other if desired)
	auto appCore = std::make_shared<Sol::Core>();

	//In this case I've configured the Core to manuallt attach LogElement as it's used everywhere for logging purposes
	if (!appCore->initialize("SolEditor-Log.txt"))
	{
		return -1; //Exit application with non-zero (i.e. abnormal) termination code
	}

	//Attach any Element(s) by name
	appCore->attachElement("Render");
	//Initialize any Element(s) by name
	//Here I'm setting the window name to "Sol Editor", width to 1280, height to 720,
	//window flags to (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), and vsync to 0 (disabled)
	appCore->getRenderElement()->initialize("Sol Editor", 1280, 720, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0);

	//Runtime loop
	bool appRunning = true;
	while (appRunning)
	{
		//To break from loop and progress to quitting application, set appRunning to false
	}
	
	//Good practice to ALWAYS
	//Call Core's terminate function in case any Elements are still attached
	appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}