#include "Core.h"

int main(int argc, char* args[]) {
	
	//Core instance
	Sol::Core editorCore;

	//Attach log element to the instance
	editorCore.attachElement("Log");

	//Error message
	editorCore.getLogElement()->logError("OHNOSPAGGETYOH");

	editorCore.~Core();

	while (true)
	{

	}
}