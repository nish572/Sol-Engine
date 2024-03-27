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

	appCore->attachElement("Shader");
	appCore->attachElement("Resource");

	appCore->attachElement("Render");
	appCore->attachElement("Gui");
	appCore->attachElement("Physics");
	appCore->attachElement("Event");
	appCore->attachElement("Ecs");

	//Initialize any Element(s) by name
	//Here I'm setting the window name to "Sol Editor", width to 1920, height to 1080,
	//window flags to (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), and vsync to 0 (disabled)
	appCore->getShaderElement()->initialize(); //Shader element does not depend upon any other element
	appCore->getResourceElement()->initialize(); //Resource element does not depend upon any other element
	appCore->getRenderElement()->initialize("Sol Editor", 1920, 1080, (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI), 0); //Render element does not depend upon any other element
	appCore->getGuiElement()->initialize(); //Gui element depends upon render element, render element must be initialized first
	appCore->getPhysicsElement()->initialize(); //Physics element//Physics element
	appCore->getEventElement()->initialize(); //Event element depends upon render element, render element must be initialized first
	appCore->getEcsElement()->initialize(true, true, false); //Ecs element depends on render element, event element, resource element, and possibly shader element

	//Runtime loop
	bool appRunning = true;
	auto testImg2 = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\fish.jpg");
	auto testImg = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\cat.jpg");
	auto testImg3 = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\dog.jpg");
	auto testImg4 = appCore->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\fullsize.jpg");

	//std::cout << "Texture IDs: " << testImg->textureID << ", " << testImg2->textureID << ", " << testImg3->textureID << std::endl;

	// Vector containing the texture pointers
	//std::vector<std::shared_ptr<TextureResource>> textures = { testImg4 };

	//for (int i = 0; i < 10; i++) {
	//	Entity newEntity = appCore->getEcsElement()->createEntity();
	//	appCore->getEcsElement()->addSprite(newEntity);
	//	appCore->getEcsElement()->addTransform(newEntity);

	//	auto& sprite2 = appCore->getEcsElement()->getSprite(newEntity);

	//	//Select random texture from the textures vector
	//	int randomTextureIndex2 = rand() % textures.size();
	//	sprite2.textureID = textures[randomTextureIndex2]->textureID;
	//	auto& trans2 = appCore->getEcsElement()->getTransform(newEntity);

	//	//For the position, assign a random float within some range
	//	float posX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f;
	//	float posY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f;
	//	std::cout << i << std::endl;

	//	trans2.position = glm::vec3(posX, posY, 0);
	//	trans2.scale = glm::vec3(100, 100, 100);
	//	std::cout << "Dog: " << i << ", textureIndex: " << randomTextureIndex2 << ", textureID: " << sprite2.textureID << std::endl;
	//}

	std::shared_ptr<TextureResource> currentImg = testImg3;

	Entity dogEntity = appCore->getEcsElement()->createEntity();
	appCore->getEcsElement()->addSprite(dogEntity);
	appCore->getEcsElement()->addTransform(dogEntity);
	appCore->getEcsElement()->addCollider(dogEntity);
	appCore->getEcsElement()->addPhysicsBody(dogEntity);

	auto& sprite = appCore->getEcsElement()->getSprite(dogEntity);
	sprite.textureID = currentImg->textureID;
	sprite.size = glm::vec2 (currentImg->width, currentImg->height);
	auto& trans = appCore->getEcsElement()->getTransform(dogEntity);
	trans.position = glm::vec3(300, 500, 0);
	trans.scale = glm::vec3(0.2, 0.2, 0);
	auto& collider = appCore->getEcsElement()->getCollider(dogEntity);
	//b2Shape* colliderShape, float colliderDensity, float colliderFriction, float colliderRestitution
	b2PolygonShape* square = new b2PolygonShape(); //Be careful where I call this since if it is out of scope, it's lifetime is over and the physics won't work
	square->SetAsBox(3.06f, 2.04f); // Half-width and half-height
	collider.shape = square;
	auto& physBod = appCore->getEcsElement()->getPhysicsBody(dogEntity);
	physBod.type = BodyType::Dynamic;

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