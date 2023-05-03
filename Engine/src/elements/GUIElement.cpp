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
	bool GuiElement::initialize()
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

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
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

		// UPDATE Gui ELEMENTS IN THIS FUNCTION //
		updateImGuiWindows();

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

	void GuiElement::updateImGuiWindows()
	{
		//ImGui SCAFFOLD APP HERE //
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

			if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::End();
		}
	}

	void GuiElement::terminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}