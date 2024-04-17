#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>
#include <string>

#include "debuglog/LogElement.h"
#include "render/RenderElement.h"
#include "render/GuiElement.h"
#include "physics/PhysicsElement.h"
#include "event/EventElement.h"
#include "resource/ResourceElement.h"
#include "resource/ShaderElement.h"
#include "ecs/EcsElement.h"
#include "ecs/Components.h"
#include "scene/SceneElement.h"

#include "Config.h"

namespace Sol
{
	//Core represents the central manager of the Sol Engine
	//Responsible for initializing, updating, and terminating the Elements
	//Responsible for allowing any Sol Applications such as the Editor to interact with the Engine (Core, Elements, Ecs)
	class Core : public std::enable_shared_from_this<Core>
	{
	public:
		//Initialize pointers to Elements with new instances of Elements
		ENGINE_API Core();
		//Release resources associated with the Core instance
		ENGINE_API ~Core();

		//Attach an Element in application runtime
		//To make an Element usable by the Core, amend the attachElement code to include the Element
		//elementName should only be in Capitalization case e.g. 'Render', and should not include 'Element'
		ENGINE_API bool attachElement(const std::string& elementName);

		//Detach an Element
		//Preferably, do so ONLY from Core's terminate function as this checks to see if each Element is not a nullptr
		//i.e. if the Element is still attached, and if so, call Element's terminate function
		//which automatically calls detachElement once Element's termination complete
		//However, if detachElement call desired prior to terminating Core, ensure the Element's terminate function has been called first
		ENGINE_API bool detachElement(const std::string& elementName);

		//Initialize Core
		//Return true/false to be handled by the application
		ENGINE_API bool initialize();

		//Update Core
		ENGINE_API void run();

		//Call terminate functions for all Elements, first checking they're not nullptrs
		ENGINE_API void terminate();

		//---

		//Get functions for Elements

		//Return a pointer to the LogElement instance managed by the Core
		ENGINE_API CoreLogElement::LogElement* getLogElement() const;

		//Return a pointer to the RenderElement instance managed by the Core
		ENGINE_API CoreRenderElement::RenderElement* getRenderElement() const;

		//Return a pointer to the GuiElement instance managed by the Core
		ENGINE_API CoreGuiElement::GuiElement* getGuiElement() const;

		//Return a pointer to the EventElement instance managed by the Core
		ENGINE_API CoreEventElement::EventElement* getEventElement() const;

		//Return a pointer to the PhysicsElement instance managed by the Core
		ENGINE_API CorePhysicsElement::PhysicsElement* getPhysicsElement() const;

		//Return a pointer to the ResourceElement instance managed by the Core
		ENGINE_API CoreResourceElement::ResourceElement* getResourceElement() const;

		//Return a pointer to the ShaderElement instance managed by the Core
		ENGINE_API CoreShaderElement::ShaderElement* getShaderElement() const;

		//Return a pointer to the EcsElement instance managed by the Core
		ENGINE_API std::shared_ptr<CoreEcsElement::EcsElement> getEcsElement() const;

		//Return a pointer to the SceneElement instance managed by the Core
		ENGINE_API std::shared_ptr<CoreSceneElement::SceneElement> getSceneElement() const;

		//---

	private:
		//Smart pointers provide automatic memory management
		//When Core instance is deleted, Elements also automatically deleted (and memory freed)
		//Additionally, if an exception is thrown during the initialization of Core, Element objects automatically deleted
		//This ensures no memory leaks there
		
		//Smart pointer(s) for an instance of the each Element
		//--- 
		std::unique_ptr<CoreLogElement::LogElement> m_logElement;
		std::unique_ptr<CoreRenderElement::RenderElement> m_renderElement;
		std::unique_ptr<CoreGuiElement::GuiElement> m_guiElement;
		std::unique_ptr<CoreEventElement::EventElement> m_eventElement;
		std::unique_ptr<CorePhysicsElement::PhysicsElement> m_physicsElement;
		std::unique_ptr<CoreResourceElement::ResourceElement> m_resourceElement;
		std::unique_ptr<CoreShaderElement::ShaderElement> m_shaderElement;
		std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;
		std::shared_ptr<CoreSceneElement::SceneElement> m_sceneElement;
		//---
	};
}