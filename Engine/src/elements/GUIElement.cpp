//------- GUI Element -------
//Forms the Editor Interface
//For The Sol Core Editor
//---------------------------

#include "render/GuiElement.h"
#include "Config.h"

#include "Core.h"

namespace CoreGuiElement
{
	GuiElement::GuiElement(std::shared_ptr<Sol::Core> core) : m_core(core), m_debugMode(true)
	{
	}
	GuiElement::~GuiElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, the debug mode
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
				//The GUI Element requires the Render Element for an OpenGL context to render GUI to
				//If not present, then log/output the appropriate error
				if (m_logElementAttached)
				{
					std::cerr << "Failed to initialize GuiElement: RenderElement is a nullptr" << std::endl;
					return false;
				}
				std::cerr << "Failed to initialize GuiElement: RenderElement is a nullptr" << std::endl;
				return false;
			}
		}

		//Check if the Editor should be viewed at all, if debug is false then no, otherwise yes (defaults to yes anyways)
		m_debugMode = debug;

		//Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     //Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      //Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         //Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       //Enable Multi-Viewport / Platform Windows

		//Setup Dear ImGui style (easy to switch to light mode however some widgets appear too light, so dark mode only for now)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		//When viewports are enabled, tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Setup Platform/Renderer backends
		//In this case, OpenGL 3.3 with SDL2
		if (corePtr)
		{
			//If Render Element is not attached to Core, then GUI Element cannot be initialized, hence error check earlier
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

	//Current implementation to render and process the Editor GUI:
	//Too tightly coupled, but the only working solution right now due to the nature of Dear ImGui
	//Essentially include a debug flag
	//When debug is enabled, editor viewports appear
	//When debug is disabled, editor viewports do not appear

	void GuiElement::update(double deltaTime)
	{
		//Get ImGui input data
		ImGuiIO& io = ImGui::GetIO();

		//Use the provided deltaTime value to set the value for ImGui
		io.DeltaTime = static_cast<float>(deltaTime);

		//Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//If Editor should be present, call a function that handles all viewports
		if (m_debugMode)
		{
			editorViewports();
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

	//The popup to show upon first starting the Editor
	void GuiElement::startUpDialog() {
		static bool openPopup = true;
		static bool showLoadOptions = false;

		if (openPopup) {
			ImGui::OpenPopup("StartUp Options");
			//Once popup has been executed once, it is closed without another way to re-open
			//This is intended otherwise this popup will show constantly
			openPopup = false;
		}

		//Initial popup window
		if (ImGui::BeginPopupModal("StartUp Options", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (!showLoadOptions) {
				if (ImGui::Button("Load Existing Project")) {
					showLoadOptions = true;
				}
				ImGui::SameLine();
			}

			if (ImGui::Button("Create New Project")) {
				//If the user selects to create a new project, then the project settings popup is displayed to allow configuring the settings for this project
				ApplicationConfig::Config::setProjectName("");
				ApplicationConfig::Config::setProjectPath("");
				m_openProjectSettingsPopup = true;
				handleProjectSettingsPopup();
				ImGui::CloseCurrentPopup();
				auto corePtr = m_core.lock();
				if (corePtr) {
					corePtr->getRenderElement()->setWindowSize(ApplicationConfig::Config::screenWidth, ApplicationConfig::Config::screenHeight);
					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr) {
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
						m_currentPath = ApplicationConfig::Config::projectPath;
						m_sceneLoadPath = ApplicationConfig::Config::projectPath;
					}
				}
				showLoadOptions = false;
			}

			if (showLoadOptions) {
				//If the user selects to load an existing project, then the user can set the filepath of the project to load
				static char filePath[1024];
				if (ImGui::InputText("Project Path", filePath, IM_ARRAYSIZE(filePath)))
				{
					std::string path(filePath);
					if (!path.empty() && path.front() == '"') {
						path.erase(0, 1);
					}
					if (!path.empty() && path.back() == '"') {
						path.pop_back();
					}
					strncpy_s(filePath, sizeof(filePath), path.c_str(), _TRUNCATE);
				}
				if (ImGui::Button("Load")) {
					ApplicationConfig::Config::setProjectPath(filePath);
					ApplicationConfig::Config::loadConfig();
					ImGui::CloseCurrentPopup();
					auto corePtr = m_core.lock();
					if (corePtr) {
						corePtr->getRenderElement()->setWindowSize(ApplicationConfig::Config::screenWidth, ApplicationConfig::Config::screenHeight);
						auto ecsPtr = corePtr->getEcsElement();
						if (ecsPtr) {
							ecsPtr->setSceneRunning(false);
							m_isSceneRunning = false;
							m_currentPath = ApplicationConfig::Config::projectPath;
							m_sceneLoadPath = ApplicationConfig::Config::projectPath;
						}
					}
					showLoadOptions = false;
				}
			}

			ImGui::EndPopup();
		}
	}

	//Handles the displaying of all viewports, including the start up dialog, the main dockable viewport, and any sub-viewports
	//To be called in the GUI Element's update function during an ImGui frame
	void GuiElement::editorViewports()
	{
		startUpDialog();
		mainEditorViewport();
		sceneHierarchyViewport();
		inspectorViewport();
		resourceBrowserViewport();
	}

	//Auxilliary function to be clean a Component type name to allow this to be written cleanly where required in the Editor
	std::string GuiElement::cleanTypeName(const std::string& typeName) {
		//Regular expression to remove common C++ type prefixes
		std::regex prefixPattern(R"(^\s*(struct|class|enum)\s+)");
		//Use regex_replace to strip off unwanted prefixes
		return std::regex_replace(typeName, prefixPattern, "");
	}

	//Presents the main dockable viewport with a toolbar, from which further popups may appear based upon user selection
	void GuiElement::mainEditorViewport()
	{
		ImGui::Begin("Sol Editor", NULL, ImGuiWindowFlags_MenuBar);
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

		//Menu for scene management, project settings viewing/modification, and for the buttons to start/stop/save the scene in the playtest window
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("New Scene")) {
					//Create new scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							m_openNewScenePopup = true;
						}
					}
				}
				if (ImGui::MenuItem("Load Scene")) {
					//Load scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							m_openLoadScenePopup = true;
						}
					}
				}
				if (ImGui::MenuItem("Save Scene")) {
					//Save current scene
					auto corePtr = m_core.lock();
					if (corePtr)
					{
						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr)
						{
							m_openSaveScenePopup = true;
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
			ImGui::Separator();
			if (ImGui::Button("Start"))
			{
				//Rendering will always occur, however starting physics and events is useful for playtesting
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
				//Stop current scene (will still be rendered, however physics and events will halt)
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
						std::string currentScenePath = scenePtr->getCurrentScenePath();
						scenePtr->unloadScene(currentScenePath);
						scenePtr->loadScene(currentScenePath);
					}
				}
			}
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SameLine();
			if (ImGui::Button("Reload Scene")) {
				//Reload current scene
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
						if (m_sceneLoadPath != "")
						{
							scenePtr->loadScene(scenePtr->getCurrentScenePath());
						}
					}
				}
			}
		}
		ImGui::EndMenuBar();

		//Handle any popups from the menu bar
		handleNewScenePopup();
		handleLoadScenePopup();
		handleSaveScenePopup();
		handleProjectSettingsPopup();

		ImGui::End();
	}

	//Upon new scene being selected in the toolbar, the user will be presented with a popup
	//In this popup the user can set a name for the new scene and save, or cancel their actions
	void GuiElement::handleNewScenePopup()
	{
		if (m_openNewScenePopup) {
			ImGui::OpenPopup("Save New Scene");
			m_openNewScenePopup = false;
		}

		if (ImGui::BeginPopupModal("Save New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			static char filename[128] = "";
			ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));
			if (ImGui::Button("Save")) {
				m_selectedEntity = std::numeric_limits<std::uint32_t>::max();
				m_selectedComponentType = typeid(void);
				std::string baseDir = ApplicationConfig::Config::projectPath;
				if (baseDir.back() != '\\' && baseDir.back() != '/') {
					baseDir += '\\';
				}
				std::string filePath = baseDir + std::string(filename) + ".scn";
				auto corePtr = m_core.lock();
				if (corePtr) {
					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
					}
					auto scenePtr = corePtr->getSceneElement();
					if (scenePtr) {
						scenePtr->newScene();
						scenePtr->unloadScene(filePath);
						scenePtr->loadScene(filePath);
					}
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	//Upon load scene being selected in the toolbar, the user will be presented with a popup
	//In this popup the user can browse the project directory to find a scene to load, and load this, or cancel their action
	void GuiElement::handleLoadScenePopup()
	{
		if (m_openLoadScenePopup) {
			ImGui::OpenPopup("Load Existing Scene");
			m_openLoadScenePopup = false;
		}

		if (ImGui::BeginPopupModal("Load Existing Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			//Navigation and current path display at the top
			if (ImGui::Button("Up")) {
				fs::path currentPath(m_sceneLoadPath);
				fs::path rootPath(ApplicationConfig::Config::projectPath);
				currentPath = fs::absolute(currentPath);
				rootPath = fs::absolute(rootPath);

				if (currentPath.has_parent_path() && fs::equivalent(currentPath.parent_path(), rootPath)) {
					m_sceneLoadPath = rootPath.string();
				}
				else if (currentPath.has_parent_path() && currentPath.parent_path() != rootPath && currentPath > rootPath) {
					m_sceneLoadPath = currentPath.parent_path().string();
				}
			}
			ImGui::SameLine();
			ImGui::Text("Current Directory: %s", m_sceneLoadPath.c_str());

			ImGui::Separator();

			//Set up columns for directories and files
			ImGui::Columns(2, nullptr, false);

			ImGui::Text("Directories");
			for (const auto& entry : fs::directory_iterator(m_sceneLoadPath)) {
				if (fs::is_directory(entry)) {
					if (ImGui::Button((entry.path().filename().string() + "\\").c_str())) {
						m_sceneLoadPath = entry.path().string();
					}
				}
			}
			ImGui::NextColumn();
			ImGui::Text("Scene Files");
			for (const auto& entry : fs::directory_iterator(m_sceneLoadPath)) {
				if (entry.path().extension() == ".scn") {
					if (ImGui::Button(entry.path().filename().string().c_str())) {
						m_selectedScene = entry.path().string();
						ImGui::OpenPopup("Confirm Load");
					}
				}
			}

			ImGui::Columns(1);
			ImGui::Separator();
			
			//An additional popup will presented to confirm the loading of a scene and to remind the user to go back and save their current scene should they wish
			if (ImGui::BeginPopupModal("Confirm Load", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				fs::path sceneName = fs::path(m_selectedScene).filename();
				ImGui::Text("Scene To Load %s", sceneName.string().c_str());

				ImGui::Separator();

				ImGui::Text("Are you sure you want to load this scene?");
				ImGui::Text("Remember to save your current work if necessary.");
				if (ImGui::Button("Yes, Load")) {
					m_selectedEntity = std::numeric_limits<std::uint32_t>::max();
					m_selectedComponentType = typeid(void);
					auto corePtr = m_core.lock();

					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
					}

					if (corePtr) {
						auto scenePtr = corePtr->getSceneElement();
						if (scenePtr) {
							scenePtr->loadScene(m_selectedScene);
						}
					}

					ImGui::CloseCurrentPopup();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	
	//Upon save scene being selected in the toolbar, the user will be presented with a popup
	//In this popup the user can choose to save the current scene, or cancel their action
	void GuiElement::handleSaveScenePopup()
	{
		if (m_openSaveScenePopup) {
			ImGui::OpenPopup("Save Scene Confirmation");
			m_openSaveScenePopup = false;
		}

		//Popup modal for save confirmation
		if (ImGui::BeginPopupModal("Save Scene Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to save changes to the current scene?\nThis operation cannot be undone!\n\n");
			ImGui::Separator();

			if (ImGui::Button("Yes", ImVec2(120, 0))) {
				auto corePtr = m_core.lock();
				if (corePtr) {

					auto ecsPtr = corePtr->getEcsElement();
					if (ecsPtr)
					{
						ecsPtr->setSceneRunning(false);
						m_isSceneRunning = false;
					}

					auto scenePtr = corePtr->getSceneElement();
					if (scenePtr) {
						std::string currentScenePath = scenePtr->getCurrentScenePath();
						scenePtr->unloadScene(currentScenePath);
						scenePtr->loadScene(currentScenePath);
					}
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	//Upon project settings being selected in the toolbar, the user will be presented with a popup
	//In this popup the user can view and save any modified project settings (held by Context.txt), or cancel their actions
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
			strcpy_s(pathBuffer, sizeof(pathBuffer), ApplicationConfig::Config::projectPath.c_str());

			if (ImGui::InputText("Project Path", pathBuffer, sizeof(pathBuffer))) {
				std::string path(pathBuffer);

				if (!path.empty() && path.front() == '"') {
					path.erase(0, 1);
				}

				if (!path.empty() && path.back() == '"') {
					path.pop_back();
				}

				strncpy_s(pathBuffer, sizeof(pathBuffer), path.c_str(), _TRUNCATE);

				ApplicationConfig::Config::setProjectPath(path);

				m_currentPath = ApplicationConfig::Config::projectPath;
				m_sceneLoadPath = ApplicationConfig::Config::projectPath;
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

	//Viewport to provide the scene hierarchy and to provide a button to add new Entities
	//By default the addition of an Entity adds a sprite and a transform, however these can be deleted after
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
					 ecsPtr->addSprite(newEntity);
				}
			}
		}
		ImGui::Separator();
		renderSceneHierarchy();
		ImGui::End();
	}

	//For use by the scene hierarchy viewport
	//Provides the display of all Entities in a scene as a tree such that Components can be visible by toggling an Entity
	//Additionally, allows Components to be added/removed from an Entity, as well as a delete button for the Entity via this popup
	void GuiElement::renderSceneHierarchy()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			auto ecsPtr = corePtr->getEcsElement();
			if (ecsPtr)
			{
				auto entityMap = ecsPtr->getEntityMap();
				//Loop through each Entity and its Components map
				for (auto& entityComponents : entityMap) {
					Entity entity = entityComponents.first;
					auto& components = entityComponents.second;

					//Format Entity identifier as Entity followed by a number
					std::string entityLabel = "Entity " + std::to_string(entity);
					if (ImGui::TreeNode(entityLabel.c_str())) {
						//Loop through components of this Entity
						for (auto& componentPair : components) {
							std::type_index componentType = componentPair.first;
							//Display Component type using a cleaned version of the Component type name
							std::string typeName = cleanTypeName(componentType.name());
							if (ImGui::Selectable(typeName.c_str())) {
								m_selectedEntity = entity;
								m_selectedComponentType = componentType;
							}
						}
						ImGui::TreePop();
					}

					//Right-click context menu for adding and removing Components
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

	//Upon the selection of Component under an Entity in the scene hierarchy viewport, that specific Component will be visible in the inspector
	//Additionally, the properties of this Component can be viewed and modified with instantaneous changes
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
							auto& component = componentIter->second;
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

								//Temporary buffer for editing text
								char buffer[256];
								//Use strncpy_s for safe copying
								strncpy_s(buffer, sizeof(buffer), spriteComponent->textureFilePath.c_str(), _TRUNCATE);

								//Editable text field for the texture file path
								if (ImGui::InputText("Texture File Path", buffer, sizeof(buffer))) {
									//Update the Component's file path and load new texture
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

								//Drag and Drop for texture file path
								//Allows dragging a resource directly from the resource browser into the text field to load
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

								//Input float to manipulate the size of a sprite
								ImGui::InputFloat2("Size", &spriteComponent->size.x);
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(PhysicsBodyComponent)) {
								auto physicsBodyComponent = std::static_pointer_cast<PhysicsBodyComponent>(component);

								ImGui::Text("Physics Body Component");
								ImGui::Separator();

								//Rotation checkbox to allow a setting of rotatable or not
								bool isRotatable = physicsBodyComponent->rotationLock;
								if (ImGui::Checkbox("Lock Rotation", &isRotatable)) {
									physicsBodyComponent->rotationLock = isRotatable;
								}

								//Body Type Selector
								const char* bodyTypes[] = { "Dynamic", "Static", "Kinematic" };
								int currentType = static_cast<int>(physicsBodyComponent->type);
								if (ImGui::Combo("Body Type", &currentType, bodyTypes, IM_ARRAYSIZE(bodyTypes))) {
									physicsBodyComponent->type = static_cast<BodyType>(currentType);
								}
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(ColliderComponent)) {
								auto colliderComponent = std::static_pointer_cast<ColliderComponent>(component);

								ImGui::Text("Collider Component");
								ImGui::Separator();

								//Visibility checkbox to allow a wireframe of the collider to be rendered
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

								//Input floats for common physics properties
								ImGui::InputFloat("Density", &colliderComponent->density);
								ImGui::InputFloat("Friction", &colliderComponent->friction);
								ImGui::InputFloat("Restitution", &colliderComponent->restitution);
								ImGui::Separator();
							}
							if (m_selectedComponentType == typeid(InputComponent)) {
								auto inputComponent = std::static_pointer_cast<InputComponent>(component);

								ImGui::Text("Input Component");
								ImGui::Separator();

								//The user can directly press a key after selecting the add action button
								//This will add an action for the input, directly associated with the key pressed by the user
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
									inputComponent->addKeyAction(addActionKey, ActionData(glm::vec2(0.0f, 0.0f), 0.0f, 0.0f, ForceType::Force, InputType::Keyboard));
									ImGui::Text("Key Captured: %s", SDL_GetKeyName(addActionKey));
								}

								ImGui::Separator();

								//Display all current input actions set for the input component, and allow their modification
								for (auto& keyAction : inputComponent->keyActions) {
									const char* keyName = SDL_GetKeyName(keyAction.first);

									int toRemove = -1;  //Index of the action to remove, -1 means no removal

									for (int i = 0; i < keyAction.second.size(); ++i) {
										ImGui::PushID(i + keyAction.first);  //Unique ID for each action
										ImGui::Text("Key: %s", keyName);
										ImGui::Text("Action:");

										//Combo for selecting ForceType
										const char* forceTypes[] = { "Force", "Impulse", "Torque", "AngularImpulse"};
										int currentForceType = static_cast<int>(keyAction.second[i].fType);
										if (ImGui::Combo("Force Type", &currentForceType, forceTypes, IM_ARRAYSIZE(forceTypes))) {
											keyAction.second[i].fType = static_cast<ForceType>(currentForceType);
										}

										//Combo for selecting InputType
										//Currently mouse movement not directly used
										const char* inputTypes[] = { "Keyboard", "MouseMovement" };
										int currentInputType = static_cast<int>(keyAction.second[i].iType);
										if (ImGui::Combo("Input Type", &currentInputType, inputTypes, IM_ARRAYSIZE(inputTypes))) {
											keyAction.second[i].iType = static_cast<InputType>(currentInputType);
										}
										//Move direction is the x,y direction to move (from -1, 0, or 1 in each axis)
										ImGui::InputFloat2("Move Direction", &keyAction.second[i].moveDirection.x);
										//Magnitude is for use if force type set to either force or impulse, and is the magnitude of the force to be applied upon the action being called
										ImGui::InputFloat("Magnitude", &keyAction.second[i].magnitude);
										//Angular magnitude is for use if force type set to either torque or angular impulse, and is the magnitude of the torque/angular impulse to be applied upon the action being called
										//Pos/neg indicates which rotational direction this should be
										ImGui::InputFloat("Angular Magnitude", &keyAction.second[i].angularMagnitude);

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

	//Viewport to provide the resource browser
	//Allows an 'up' navigation button but limits this to the root of the project path set in project settings
	//Displays current directory, a list of all directories (which can be selected to enter them), and all resources in that directory
	//For now, only image resources are displyed
	//These can be dragged and dropped from here into the texture filepath field of a sprite component in the inspector
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

	//Auxilliary function for use by the resource browser to display all directories and provide a button to select them
	//Uses a typically C++17 and above library Filesystem, however the std::experimental library is used to enable use in C++14
	void GuiElement::displayDirectories() {
		std::string directory = m_currentPath;
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (fs::is_directory(entry)) {
				if (ImGui::Button((entry.path().filename().string() + "\\").c_str())) {
					m_currentPath = entry.path().string();
				}
			}
		}
	}

	//Auxilliary function for use by the resource browser to display all files in the current directory and provide drag-drop for images
	//Uses a typically C++17 and above library Filesystem, however the std::experimental library is used to enable use in C++14
	void GuiElement::displayFilesInDirectory() {
		std::string directory = m_currentPath;
		static char item_path_buffer[1024];

		//A scrollable section for viewing files, akin to a user-familiar gallery
		ImGui::BeginChild("FilesScrolling", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing() * 2), true);
		int columns = int(ImGui::GetContentRegionAvail().x / 110);
		if (columns < 1) columns = 1;

		if (ImGui::BeginTable("FilesTable", columns)) {
			for (const auto& entry : fs::directory_iterator(directory)) {
				std::string ext = entry.path().extension().string();  //Extract the extension
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  //Convert to lowercase
				if (fs::is_regular_file(entry) && (ext == ".png" || ext == ".jpg" || ext == ".jpeg")) {
					ImGui::TableNextColumn();
					ImGui::PushID(entry.path().filename().string().c_str());
					if (ImGui::Button(entry.path().filename().string().c_str(), ImVec2(100, 100))) {
					}
					//Allows drag-drop for images
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						memset(item_path_buffer, 0, sizeof(item_path_buffer));
						strncpy_s(item_path_buffer, sizeof(item_path_buffer), entry.path().string().c_str(), _TRUNCATE);
						ImGui::SetDragDropPayload("ASSET_PATH", item_path_buffer, strlen(item_path_buffer) + 1);
						ImGui::Text("%s", entry.path().filename().string().c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
					//Additional tooltip to view the filepath of the hovered resource
					if (ImGui::IsItemHovered()) {
						ImGui::SetTooltip("%s", entry.path().string().c_str());
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();
	}

	//Upon termination, shutdown Dear ImGui and destroy this Dear ImGui context, to deallocate resources
	void GuiElement::terminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}