//The heart of the Sol Application
#include "Core.h"
// ---

int main(int argc, char* args[]) {
	
	//Core instance (make_shared allows Elements to have a weak_ptr to Core, important for Element's to access each other if desired)
	auto appCore = std::make_shared<Sol::Core>();

	// ---
	//Attach any Element(s) by name - case sensitive and does not include 'Element', e.g. only 'Render' not 'render' or 'RenderElement' etc
	//Pre-built in Elements use standard Capitalization case, e.g. 'Render', 'Physics'
	//Custom Elements SHOULD have names that conform to this Element naming convention
	//e.g. appCore->attachElement("Render");
	// ---
	// ---
	//Initialize any Element(s) by calling the Element's initialize function and passing any required parameters
	//e.g. appCore->getRenderElement()->initialize(...)
	// ---
	// MAKE SURE ELEMENTS ATTACHED FIRST THEN INITIALIZED, ONE BY ONE
	// ESPECIALLY IMPORTANT IF LOG ELEMENT NOT INITIALIZED BEFORE ATTACHING OTHER ELEMENTS!

	// ---
	//Elements can be manually detached since detachElement will call the Element's respective terminate function
	//This terminate function is responsible for cleaning up resources
	//HOWEVER it is not necessary to manually detach Elements since Core's terminate functionality
	//will automatically detach any attached Elements and cleanup their resources (by calling each Element's respective terminate function)
	// ---

	//Runtime loop
	bool appRunning = true;
	while (appRunning)
	{
		//To break from loop and progress to quitting application, set appRunning to false
	}
	
	//Good practice to
	//Call Core's terminate function in case any Elements are still attached
	//appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}