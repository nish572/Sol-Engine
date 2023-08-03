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
	appCore->getRenderElement()->initialize("Sol Editor", 1920, 1080, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0); //Render element does not depend upon any other element
	appCore->getGuiElement()->initialize(); //Gui element depends upon render element, render element must be initialized first
	//Physics element
	appCore->getEventElement()->initialize(); //Event element depends upon render element, render element must be initialized first
	appCore->getResourceElement()->initialize(); //Resource element does not depend upon any other element
	appCore->getShaderElement()->initialize(); //Shader element does not depend upon any other element
	appCore->getEcsElement()->initialize(true, false, false); //Ecs element depends on render element, event element, resource element, and possibly shader element

	//Runtime loop
	bool appRunning = true;
	auto testShader = appCore->getResourceElement()->loadShader("C:\\Software Development\\Sol-Engine\\Sol-Engine\\Engine\\shaders\\shader.vert", "C:\\Software Development\\Sol-Engine\\Sol-Engine\\Engine\\shaders\\shader.frag");
	std::cout << testShader->shaderProgramID;
	auto testImg = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\dogfortesting.jpg");
	auto testImg3 = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\dogfortesting3.jpg");
	auto testImg2 = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\dogfortesting2.jpg");

	//Dog 1
	Entity testEntity = appCore->getEcsElement()->createEntity();
	std::cout << "Entity 1 = " << testEntity << std::endl;
	appCore->getEcsElement()->addSprite(testEntity);
	appCore->getEcsElement()->addTransform(testEntity);
	auto& sprite = appCore->getEcsElement()->getSprite(testEntity);
	sprite.shaderProgram = testShader->shaderProgramID;
	std::cout << "Shader 1 = " << sprite.shaderProgram << std::endl;
	sprite.textureID = testImg3->textureID;
	std::cout << "TexID 1 = " << sprite.textureID << std::endl;
	auto& trans = appCore->getEcsElement()->getTransform(testEntity);
	trans.position = glm::vec3(0.9, 0.9, 0);
	std::cout << "Trans.x 1 = " << trans.position.x << std::endl;

	std::cout << std::endl << std::endl;

	//Dog 2
	Entity testEntity2 = appCore->getEcsElement()->createEntity();
	std::cout << "Entity 2 = " << testEntity2 << std::endl;
	appCore->getEcsElement()->addSprite(testEntity2);
	appCore->getEcsElement()->addTransform(testEntity2);
	auto& sprite2 = appCore->getEcsElement()->getSprite(testEntity2);
	sprite2.shaderProgram = testShader->shaderProgramID;
	std::cout << "Shader 2 = " << sprite2.shaderProgram << std::endl;
	sprite2.textureID = testImg2->textureID;
	std::cout << "TexID 2 = " << sprite2.textureID << std::endl;
	auto& trans2 = appCore->getEcsElement()->getTransform(testEntity2);
	trans2.position = glm::vec3(-0.6, -0.2, 0);
	std::cout << "Trans.x 2 = " << trans2.position.x << std::endl;

	std::cout << std::endl << std::endl;

	//Dog 3
	Entity testEntity3 = appCore->getEcsElement()->createEntity();
	std::cout << "Entity 3 = " << testEntity3 << std::endl;
	appCore->getEcsElement()->addSprite(testEntity3);
	appCore->getEcsElement()->addTransform(testEntity3);
	auto& sprite3 = appCore->getEcsElement()->getSprite(testEntity3);
	sprite3.shaderProgram = testShader->shaderProgramID;
	std::cout << "Shader 3 = " << sprite3.shaderProgram << std::endl;
	sprite3.textureID = testImg->textureID;
	std::cout << "TexID 3 = " << sprite3.textureID << std::endl;
	auto& trans3 = appCore->getEcsElement()->getTransform(testEntity3);
	trans3.position = glm::vec3(-0.8, -0.8, 0);
	std::cout << "Trans.x 3 = " << trans3.position.x << std::endl;

	//std::cout << "Entity = " << testEntity << std::endl;
	//std::cout << "Entity = " << testEntity2 << std::endl;
	//std::cout << "Entity 3 = " << testEntity3 << std::endl;

	//std::cout << "Shader 1 = " << sprite.shaderProgram << std::endl;
	//std::cout << "Shader 2 = " << sprite2.shaderProgram << std::endl;
	//std::cout << "Shader 3 = " << sprite3.shaderProgram << std::endl;

	//std::cout << "TexID 1 = " << sprite.textureID << std::endl;
	//std::cout << "TexID 2 = " << sprite2.textureID << std::endl;
	//std::cout << "TexID 3 = " << sprite3.textureID << std::endl;

	//std::cout << "Trans.x = " << trans.position.x << std::endl;
	//std::cout << "Trans.x 2 = " << trans2.position.x << std::endl;
	//std::cout << "Trans.x 3 = " << trans3.position.x << std::endl;

	while (appRunning)
	{
		//Run Core, passing event (so EventElement can process events)
		appCore->run();

		//Keep app running unless SDL_QUIT event received or SDL_WINDOWEVENT_CLOSE event received for appCore's window
		appRunning = appCore->getEventElement()->isRunning();
	}

	//Good practice to
	//Call Core's terminate function in case any Elements are still attached
	appCore->terminate();

	//Reset appCore as this will destroy appCore's instance of Core
	appCore.reset();

	//Exit application with 0 (i.e. normal) termination code
	return 0;
}