#include "render/GuiElement.h"

#include "Core.h"

namespace CoreGuiElement
{
	//GuiElement has initializer list for any managed resources that require initializing
	//All Elements MUST have at least m_core private member
	GuiElement::GuiElement(std::shared_ptr<Sol::Core> core) : m_core(core) //Extend initializer list if necessary
	{
	}
	GuiElement::~GuiElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool GuiElement::initialize(bool debug)
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}
		if (corePtr)
		{
			if (!corePtr->getRenderElement())
			{
				if (m_logElementAttached)
				{
					std::cerr << "Failed to initialize GuiElement: RenderElement is a nullptr" << std::endl;
					return false;
				}
				std::cerr << "Failed to initialize GuiElement: RenderElement is a nullptr" << std::endl;
				return false;
			}
		}

		m_debugMode = debug;

		//Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     //Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      //Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         //Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       //Enable Multi-Viewport / Platform Windows

		//Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Setup Platform/Renderer backends
		if (corePtr)
		{
			//If RenderElement is not attached to Core, then GuiElement cannot be initialized
			ImGui_ImplSDL2_InitForOpenGL(corePtr->getRenderElement()->getWindow(), corePtr->getRenderElement()->getGLContext());
			ImGui_ImplOpenGL3_Init("#version 330");
		}

		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Gui] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Gui] Successfully Initialized" << std::endl;
		return true;
	}

	//Current plan:
	//Too tightly coupled, but the only working solution right now due to the nature of ImGui
	//Essentially include a debug flag
	//When debug is enabled, editor viewports appear and create a new scene (or load existing scene of existing project, but default to new scene I think)
	//When debug is disabled, editor viewports do not appear and instead load the first scene from the serialised scene data

	//Update
	void GuiElement::update(double deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO();

		//Use the provided deltaTime value
		io.DeltaTime = static_cast<float>(deltaTime);

		//Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Create dockable background space for all viewports
		//ImGui::DockSpaceOverViewport();

		if (m_debugMode)
		{
			editorViewports();
		}
		else
		{
			;
		}

		ImGui::Render();

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}		
	}

	void GuiElement::editorViewports()
	{
		mainEditorViewport();
		sceneHierarchyViewport();
		inspectorViewport();
		sceneToolbar();
		debuggerViewport();
		resourceBrowserViewport();
	}

	void GuiElement::mainEditorViewport()
	{
		ImGui::Begin("Sol Editor", NULL, ImGuiWindowFlags_MenuBar);
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	void GuiElement::sceneToolbar()
	{
		ImGui::Begin("Scene Toolbar");
		ImGui::Button("Start");
		ImGui::SameLine();
		ImGui::Button("Stop");
		ImGui::SameLine();
		ImGui::Button("Save");
		ImGui::End();
	}

	void GuiElement::sceneHierarchyViewport()
	{
		ImGui::Begin("Scene");

		if (ImGui::Button("Add Entity"))
		{
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				auto ecsPtr = corePtr->getEcsElement();
				if (ecsPtr)
				{
					Entity dogEntity = ecsPtr->createEntity();
					ecsPtr->addSprite(dogEntity);
					ecsPtr->addTransform(dogEntity);
					auto testImg = corePtr->getResourceElement()->loadTextureResource("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\fish.jpg");
					std::shared_ptr<TextureResource> currentImg = testImg;
					auto& sprite = ecsPtr->getSprite(dogEntity);
					sprite.textureID = currentImg->textureID;
					sprite.size = glm::vec2(currentImg->width, currentImg->height);
					auto& trans = ecsPtr->getTransform(dogEntity);
					trans.position = glm::vec3(0, 0, 0);
					trans.scale = glm::vec3(0.2, 0.2, 0);
					b2PolygonShape* square = new b2PolygonShape(); //Be careful where I call this since if it is out of scope, it's lifetime is over and the physics won't work
					square->SetAsBox(0.612f, 0.408f); // Half-width and half-height
					ecsPtr->addCollider(dogEntity);
					auto& collider = ecsPtr->getCollider(dogEntity);
					collider.shape = square;
					ecsPtr->addPhysicsBody(dogEntity);
					auto& physBod = ecsPtr->getPhysicsBody(dogEntity);
					physBod.type = BodyType::Dynamic;
				}
			}
		}

		ImGui::End();
	}

	void GuiElement::inspectorViewport()
	{
		ImGui::Begin("Inspector");
		ImGui::End();
	}

	void GuiElement::debuggerViewport()
	{
		ImGui::Begin("Debug Log");
		ImGui::End();
	}
	void GuiElement::resourceBrowserViewport()
	{
		ImGui::Begin("Resource Browser");
		ImGui::End();
	}

	void GuiElement::terminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}