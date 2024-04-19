#include "render/GuiElement.h"
#include "Config.h"

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

		if (m_debugMode)
		{
			ApplicationConfig::Config::loadConfig();
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				corePtr->getRenderElement()->setWindowSize(ApplicationConfig::Config::screenWidth, ApplicationConfig::Config::screenHeight);
				auto ecsPtr = corePtr->getEcsElement();
				if (ecsPtr)
				{
					ecsPtr->setSceneRunning(false);
					m_isSceneRunning = false;
					m_currentPath = ApplicationConfig::Config::projectPath;
				}
			}
		}

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
		resourceBrowserViewport();
	}

	std::string GuiElement::cleanTypeName(const std::string& typeName) {
		//Regular expression to remove common C++ type prefixes
		std::regex prefixPattern(R"(^\s*(struct|class|enum)\s+)");
		//Use regex_replace to strip off unwanted prefixes
		return std::regex_replace(typeName, prefixPattern, "");
	}

	void GuiElement::mainEditorViewport()
	{
		ImGui::Begin("Sol Editor", NULL, ImGuiWindowFlags_MenuBar);
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("New Scene")) {
					//Create new scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto ecsPtr = corePtr->getEcsElement();
						if (ecsPtr)
						{
							ecsPtr->setSceneRunning(false);
							m_isSceneRunning = false;
						}

						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							scenePtr->newScene();
						}
					}
					m_selectedEntity = std::numeric_limits<std::uint32_t>::max();
					m_selectedComponentType = typeid(void);
				}
				if (ImGui::MenuItem("Load Scene")) {
					//Load scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto ecsPtr = corePtr->getEcsElement();
						if (ecsPtr)
						{
							ecsPtr->setSceneRunning(false);
							m_isSceneRunning = false;
						}

						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							scenePtr->loadScene("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\test.scn");
						}
					}
					m_selectedEntity = std::numeric_limits<std::uint32_t>::max();
					m_selectedComponentType = typeid(void);
				}
				if (ImGui::MenuItem("Save Scene")) {
					//Save current scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto ecsPtr = corePtr->getEcsElement();
						if (ecsPtr)
						{
							ecsPtr->setSceneRunning(false);
							m_isSceneRunning = false;
						}

						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							scenePtr->unloadScene("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\test.scn");
						}
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project")) {
				if (ImGui::MenuItem("Project Settings")) {
					//Load config page for project settings
					m_openProjectSettingsPopup = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Open Documentation")) {
					//Load the text file with the documentation
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::Button("Start"))
			{
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(true);
						m_isSceneRunning = true;
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				//Save current scene
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
					}

					auto scenePtr = corePtr->getSceneElement();
					if (scenePtr)
					{
						scenePtr->unloadScene("C:\\Software Development\\Sol-Engine\\Sol-Engine\\downloads\\test.scn");
					}
				}
			}
		}
		ImGui::EndMenuBar();
		handleProjectSettingsPopup();
		ImGui::End();
	}

	void GuiElement::handleProjectSettingsPopup()
	{
		if (m_openProjectSettingsPopup) {
			ImGui::OpenPopup("Project Settings");
		}

		if (ImGui::BeginPopupModal("Project Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Edit Project Settings");
			static char nameBuffer[128];
			strcpy_s(nameBuffer, ApplicationConfig::Config::projectName.c_str());
			if (ImGui::InputText("Project Name", nameBuffer, sizeof(nameBuffer))) {
				ApplicationConfig::Config::setProjectName(std::string(nameBuffer));
			}

			static char pathBuffer[256];
			strcpy_s(pathBuffer, ApplicationConfig::Config::projectPath.c_str());
			if (ImGui::InputText("Project Path", pathBuffer, sizeof(pathBuffer))) {
				ApplicationConfig::Config::setProjectPath(std::string(pathBuffer));
			}

			static float width = ApplicationConfig::Config::screenWidth;
			static float height = ApplicationConfig::Config::screenHeight;
			if (ImGui::InputFloat("Game Width", &width)) {
				ApplicationConfig::Config::setScreenSize(width, height);
			}
			if (ImGui::InputFloat("Game Height", &height)) {
				ApplicationConfig::Config::setScreenSize(width, height);
			}

			if (ImGui::Button("Save")) {
				ApplicationConfig::Config::saveConfig();
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					corePtr->getRenderElement()->setWindowSize(ApplicationConfig::Config::screenWidth, ApplicationConfig::Config::screenHeight);
				}
				ImGui::CloseCurrentPopup();
				m_openProjectSettingsPopup = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
				m_openProjectSettingsPopup = false;
			}
			ImGui::EndPopup();
		}
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
					 auto newEntity = ecsPtr->createEntity();
					 ecsPtr->addTransform(newEntity);
				}
			}
		}
		ImGui::Separator();
		renderSceneHierarchy();
		ImGui::End();
	}

	void GuiElement::renderSceneHierarchy()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			auto ecsPtr = corePtr->getEcsElement();
			if (ecsPtr)
			{
				auto entityMap = ecsPtr->getEntityMap();
				//Loop through each entity and its components map
				for (auto& entityComponents : entityMap) {
					Entity entity = entityComponents.first;
					auto& components = entityComponents.second;

					//Format entity identifier as Entity followed by a number
					std::string entityLabel = "Entity " + std::to_string(entity);
					if (ImGui::TreeNode(entityLabel.c_str())) {
						//Loop through components of this entity
						for (auto& componentPair : components) {
							std::type_index componentType = componentPair.first;
							//Display component type directly using the type name
							std::string typeName = cleanTypeName(componentType.name());
							if (ImGui::Selectable(typeName.c_str())) {
								m_selectedEntity = entity;
								m_selectedComponentType = componentType;
							}
						}
						ImGui::TreePop();
					}

					//Right-click context menu for adding and removing components
					if (ImGui::BeginPopupContextItem(entityLabel.c_str())) {
						ImGui::Text("Add/Remove Components: ");
						ImGui::Separator();
						if (components.find(std::type_index(typeid(TransformComponent))) == components.end())
						{
							if (ImGui::MenuItem("Add Transform Component"))
							{
								ecsPtr->addTransform(entity);
							}
						}
						else
						{
							if (ImGui::MenuItem("Delete Transform Component"))
							{
								ecsPtr->removeTransform(entity);
							}
						}
						if (components.find(std::type_index(typeid(SpriteComponent))) == components.end())
						{
							if (ImGui::MenuItem("Add Sprite Component"))
							{
								ecsPtr->addSprite(entity);
							}
						}
						else
						{
							if (ImGui::MenuItem("Delete Sprite Component"))
							{
								ecsPtr->removeSprite(entity);
							}
						}
						if (components.find(std::type_index(typeid(PhysicsBodyComponent))) == components.end())
						{
							if (ImGui::MenuItem("Add Physics Component"))
							{
								ecsPtr->addPhysicsBody(entity);
							}
						}
						else
						{
							if (ImGui::MenuItem("Delete Physics Component"))
							{
								ecsPtr->removePhysicsBody(entity);
							}
						}
						if (components.find(std::type_index(typeid(ColliderComponent))) == components.end())
						{
							if (ImGui::MenuItem("Add Collider Component"))
							{
								ecsPtr->addCollider(entity);
							}
						}
						else
						{
							if (ImGui::MenuItem("Delete Collider Component"))
							{
								ecsPtr->removeCollider(entity);
							}
						}
						if (components.find(std::type_index(typeid(InputComponent))) == components.end())
						{
							if (ImGui::MenuItem("Add Input Component"))
							{
								ecsPtr->addInput(entity);
							}
						}
						else
						{
							if (ImGui::MenuItem("Delete Input Component"))
							{
								ecsPtr->removeInput(entity);
							}
						}
						ImGui::Separator();
						if (ImGui::Button("Delete Entity"))
						{
							ecsPtr->destroyEntity(entity);
						}
						ImGui::EndPopup();
					}
				}
			}
		}
	}

	void GuiElement::inspectorViewport()
	{
		ImGui::Begin("Inspector");
		auto corePtr = m_core.lock();
		if (corePtr) {
			auto ecsPtr = corePtr->getEcsElement();
			if (ecsPtr) {
				if (m_selectedEntity != std::numeric_limits<std::uint32_t>::max() && m_selectedComponentType != typeid(void))
				{
					auto entityMap = ecsPtr->getEntityMap();
					auto entityIter = entityMap.find(m_selectedEntity);
					if (entityIter != entityMap.end()) {
						auto& components = entityIter->second;
						auto componentIter = components.find(m_selectedComponentType);
						if (componentIter != components.end()) {
							auto component = componentIter->second;
							if (m_selectedComponentType == typeid(TransformComponent))
							{
								auto transformComponent = std::static_pointer_cast<TransformComponent>(component);

								ImGui::Text("Transform Component");
								ImGui::Separator();

								//Position Inputs
								ImGui::InputFloat("Position X", &transformComponent->position.x);
								ImGui::InputFloat("Position Y", &transformComponent->position.y);
								ImGui::Separator();

								//Scale Inputs
								ImGui::InputFloat("Scale X", &transformComponent->scale.x);
								ImGui::InputFloat("Scale Y", &transformComponent->scale.y);
								ImGui::Separator();

								//Rotation Input
								ImGui::InputFloat("Rotation", &transformComponent->rotation);
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(SpriteComponent)) {
								auto spriteComponent = std::static_pointer_cast<SpriteComponent>(component);

								ImGui::Text("Sprite Component");
								ImGui::Separator();

								// Temporary buffer for editing text
								char buffer[256];
								// Use strncpy_s for safe copying
								strncpy_s(buffer, sizeof(buffer), spriteComponent->textureFilePath.c_str(), _TRUNCATE);

								// Editable text field for the texture file path
								if (ImGui::InputText("Texture File Path", buffer, sizeof(buffer))) {
									// Update the component's file path and load new texture
									spriteComponent->textureFilePath = buffer;
									auto resourceElement = corePtr->getResourceElement();
									if (resourceElement) {
										auto newSprite = resourceElement->loadTextureResource(buffer);
										if (newSprite) {
											spriteComponent->textureID = newSprite->textureID;
											spriteComponent->size = glm::vec2(newSprite->width, newSprite->height);
											spriteComponent->color = glm::vec4(1.0f);
										}
										else {
											ImGui::Text("Failed to load texture!");
										}
									}
								}

								// Drag and Drop for texture file path
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
										strncpy_s(buffer, sizeof(buffer), (const char*)payload->Data, _TRUNCATE);
										spriteComponent->textureFilePath = buffer;
										auto resourceElement = corePtr->getResourceElement();
										if (resourceElement) {
											auto newSprite = resourceElement->loadTextureResource(buffer);
											if (newSprite) {
												spriteComponent->textureID = newSprite->textureID;
												spriteComponent->size = glm::vec2(newSprite->width, newSprite->height);
												spriteComponent->color = glm::vec4(1.0f);
											}
											else {
												ImGui::Text("Failed to load texture!");
											}
										}
									}
									ImGui::EndDragDropTarget();
								}

								ImGui::InputFloat2("Size", &spriteComponent->size.x);
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(PhysicsBodyComponent)) {
								auto physicsBodyComponent = std::static_pointer_cast<PhysicsBodyComponent>(component);

								ImGui::Text("Physics Body Component");
								ImGui::Separator();

								//Body Type Selector
								const char* bodyTypes[] = { "Dynamic", "Static", "Kinematic" };
								int currentType = static_cast<int>(physicsBodyComponent->type);
								if (ImGui::Combo("Body Type", &currentType, bodyTypes, IM_ARRAYSIZE(bodyTypes))) {
									physicsBodyComponent->type = static_cast<BodyType>(currentType);
								}

								//Other properties like body mass, density, friction etc. could be edited if relevant and exposed
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(ColliderComponent)) {
								auto colliderComponent = std::static_pointer_cast<ColliderComponent>(component);

								ImGui::Text("Collider Component");
								ImGui::Separator();

								bool isVisible = colliderComponent->isVisible;
								if (ImGui::Checkbox("Show Collision Box", &isVisible)) {
									colliderComponent->isVisible = isVisible;
								}

								//Shape type selector
								const char* items[] = { "Box", "Circle" };
								int currentItem = static_cast<int>(colliderComponent->shapeType);
								if (ImGui::Combo("Shape Type", &currentItem, items, IM_ARRAYSIZE(items))) {
									colliderComponent->shapeType = static_cast<ShapeType>(currentItem);
								}

								if (colliderComponent->shapeType == ShapeType::Box) {
									//Box dimensions
									ImGui::InputFloat("Width", &colliderComponent->width);
									ImGui::InputFloat("Height", &colliderComponent->height);
								}
								else if (colliderComponent->shapeType == ShapeType::Circle) {
									//Circle dimensions
									ImGui::InputFloat("Radius", &colliderComponent->radius);
								}

								//Common physics properties
								ImGui::InputFloat("Density", &colliderComponent->density);
								ImGui::InputFloat("Friction", &colliderComponent->friction);
								ImGui::InputFloat("Restitution", &colliderComponent->restitution);
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(InputComponent)) {
								auto inputComponent = std::static_pointer_cast<InputComponent>(component);

								ImGui::Text("Input Component");
								ImGui::Separator();

								SDL_Keycode addActionKey;
								if (ImGui::Button("Add Action")) {
									SDL_Event e;
									bool keyCaptured = false;
									while (!keyCaptured) {
										while (SDL_PollEvent(&e)) { //Poll SDL events
											if (e.type == SDL_KEYDOWN) { //Check for key down events
												addActionKey = e.key.keysym.sym; //Capture the pressed key
												keyCaptured = true; //Set the flag to exit the loop
												break; //Exit the inner loop
											}
										}
									}
									inputComponent->addKeyAction(addActionKey, ActionData(glm::vec2(0.0f, 0.0f), 1.0f, ForceType::Force, InputType::Keyboard));
									ImGui::Text("Key Captured: %s", SDL_GetKeyName(addActionKey));
								}

								ImGui::Separator();

								for (auto& keyAction : inputComponent->keyActions) {
									const char* keyName = SDL_GetKeyName(keyAction.first);

									int toRemove = -1;  //Index of the action to remove, -1 means no removal

									for (int i = 0; i < keyAction.second.size(); ++i) {
										ImGui::PushID(i + keyAction.first);  //Unique ID for each action
										ImGui::Text("Key: %s", keyName);
										ImGui::Text("Action:");

										//Combo for selecting ForceType
										const char* forceTypes[] = { "Force", "Impulse" };
										int currentForceType = static_cast<int>(keyAction.second[i].fType);
										if (ImGui::Combo("Force Type", &currentForceType, forceTypes, IM_ARRAYSIZE(forceTypes))) {
											keyAction.second[i].fType = static_cast<ForceType>(currentForceType);
										}

										//Combo for selecting InputType
										const char* inputTypes[] = { "Keyboard", "MouseMovement" };
										int currentInputType = static_cast<int>(keyAction.second[i].iType);
										if (ImGui::Combo("Input Type", &currentInputType, inputTypes, IM_ARRAYSIZE(inputTypes))) {
											keyAction.second[i].iType = static_cast<InputType>(currentInputType);
										}
										ImGui::InputFloat2("Move Direction", &keyAction.second[i].moveDirection.x);
										ImGui::InputFloat("Magnitude", &keyAction.second[i].magnitude);

										if (ImGui::Button("Remove Action")) {
											toRemove = i;
										}
										ImGui::PopID();
										ImGui::Separator();
									}

									if (toRemove != -1) {
										keyAction.second.erase(keyAction.second.begin() + toRemove);
									}
								}
							}
						}
					}
				}
			}
		}
		ImGui::End();
	}

	void GuiElement::resourceBrowserViewport() {
		ImGui::Begin("Resource Browser");

		//Navigation and current path display at the top
		if (ImGui::Button("Up")) {
			fs::path currentPath(m_currentPath);
			fs::path rootPath(ApplicationConfig::Config::projectPath);
			currentPath = fs::absolute(currentPath);
			rootPath = fs::absolute(rootPath);

			if (currentPath.has_parent_path() && fs::equivalent(currentPath.parent_path(), rootPath)) {
				m_currentPath = rootPath.string();
			}
			else if (currentPath.has_parent_path() && currentPath.parent_path() != rootPath && currentPath > rootPath) {
				m_currentPath = currentPath.parent_path().string();
			}
		}
		ImGui::SameLine();
		ImGui::Text("Current Directory: %s", m_currentPath.c_str());

		//Columns for directories and files
		ImGui::Columns(2, "FileBrowser"); //2 columns
		ImGui::Separator();

		//First column for directories
		ImGui::Text("Directories");
		displayDirectories();

		ImGui::NextColumn();

		//Second column for files
		ImGui::Text("Files");
		displayFilesInDirectory();

		ImGui::Columns(1);
		ImGui::Separator();

		ImGui::End();
	}

	void GuiElement::displayDirectories() {
		std::string directory = m_currentPath;
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (fs::is_directory(entry)) {
				if (ImGui::Button((entry.path().filename().string() + "/").c_str())) {
					m_currentPath = entry.path().string();
				}
			}
		}
	}

	void GuiElement::displayFilesInDirectory() {
		std::string directory = ApplicationConfig::Config::projectPath;
		static char item_path_buffer[1024];

		ImGui::BeginChild("FilesScrolling", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing() * 2), true);
		int columns = int(ImGui::GetContentRegionAvail().x / 110);
		if (columns < 1) columns = 1;

		if (ImGui::BeginTable("FilesTable", columns)) {
			for (const auto& entry : fs::directory_iterator(directory)) {
				if (fs::is_regular_file(entry) && (entry.path().extension() == ".png" || entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg")) {
					ImGui::TableNextColumn();
					ImGui::PushID(entry.path().filename().string().c_str());
					if (ImGui::Button(entry.path().filename().string().c_str(), ImVec2(100, 100))) {
					}
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						memset(item_path_buffer, 0, sizeof(item_path_buffer));
						strncpy_s(item_path_buffer, sizeof(item_path_buffer), entry.path().string().c_str(), _TRUNCATE);
						ImGui::SetDragDropPayload("ASSET_PATH", item_path_buffer, strlen(item_path_buffer) + 1);
						ImGui::Text("%s", entry.path().filename().string().c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) {
						ImGui::SetTooltip("%s", entry.path().string().c_str());
					}
				}
			}
			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	void GuiElement::terminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}