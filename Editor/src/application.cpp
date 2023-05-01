// ------- Sol Editor -------
// An Game Editor Utilizing
// The Sol Core
// --------------------------
#include "Core.h"

int main(int argc, char* args[]) {

	//Core instance (make_shared allows Elements to have a weak_ptr to Core, important for Element's to access each other if desired)
	auto appCore = std::make_shared<Sol::Core>();

	//In this case I've configured the Core to manually attach LogElement as it's used everywhere for logging purposes
	if (!appCore->initialize("SolEditor-Log.txt"))
	{
		return -1; //Exit application with non-zero (i.e. abnormal) termination code
	}

	//Attach any Element(s) by name
	appCore->attachElement("Render");
	appCore->attachElement("GUI");
	appCore->attachElement("Input");
	appCore->attachElement("Resource");
	appCore->attachElement("Shader");

	//Initialize any Element(s) by name
	//Here I'm setting the window name to "Sol Editor", width to 1920, height to 1080,
	//window flags to (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), and vsync to 0 (disabled)
	appCore->getRenderElement()->initialize("Sol Editor", 1080, 720, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0);
	appCore->getGUIElement()->initialize();
	appCore->getEventElement()->initialize();
	appCore->getResourceElement()->initialize();
	appCore->getShaderElement()->initialize();

	//Runtime loop
	bool appRunning = true;
	while (appRunning)
	{
		//Run Core, passing event (so EventElement can process events)
		appCore->run();

		//Keep app running unless SDL_QUIT event received or SDL_WINDOWEVENT_CLOSE event received for appCore's window
		appRunning = appCore->getEventElement()->isRunning();
	}

	//Good practice to ALWAYS
	//Call Core's terminate function in case any Elements are still attached
	appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}