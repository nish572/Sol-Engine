#include "render/GUIElement.h"

#include "Core.h"

namespace CoreGUIElement
{
	//GUIElement has initializer list for any managed resources that require initializing
	//All Elements MUST have at least m_core private member
	GUIElement::GUIElement(std::shared_ptr<Sol::Core> core) : m_core(core) //Extend initializer list if necessary
	{
	}
	GUIElement::~GUIElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool GUIElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (!corePtr->getLogElement())
			{
				std::cerr << "Failed to initialize GUIElement: LogElement is a nullptr" << std::endl;
				return false;
			}
		}
		if (corePtr)
		{
			if (!corePtr->getRenderElement())
			{
				std::cerr << "Failed to initialize RenderElement: RenderElement is a nullptr" << std::endl;
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
			//If RenderElement is not attached to Core, then GUIElement cannot be initialized
			ImGui_ImplSDL2_InitForOpenGL(corePtr->getRenderElement()->getWindow(), corePtr->getRenderElement()->getGLContext());
			ImGui_ImplOpenGL3_Init("#version 330");
		}

		if (corePtr)
		{
			corePtr->getLogElement()->logInfo("[GUI] Successfully Initialized");
		}
		return true;
	}

	//Update
	void GUIElement::update()
	{
		ImGuiIO& io = ImGui::GetIO();

		//Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		updateImGuiWindows();

		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				auto renderElement = corePtr->getRenderElement();
				if (renderElement)
				{
					SDL_GL_MakeCurrent(renderElement->getWindow(), renderElement->getGLContext());
				}
			}

		}
	}

	void GUIElement::updateImGuiWindows()
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

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Bye world, world!");
			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::End();
		}
	}

	void GUIElement::terminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}