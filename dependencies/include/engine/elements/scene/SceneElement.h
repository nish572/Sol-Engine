#pragma once

#include "EngineAPI.h"

#pragma warning(push)
#pragma warning(disable: 6001 28020 26495) //Disable numerous JSON for Modern C++ warnings - warnings considered, JSON for Modern C++ fully functional and trusted, no need for these warnings

//C++ libraries
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "ecs/Components.h"

//External library JSON for Modern C++
#include <nlohmann/json.hpp>
#pragma warning(pop)

using json = nlohmann::json;
//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreSceneElement
{
	//Represents the Element responsible for managing the scene operations in the Sol Engine
	//This includes serialisation/deserialisation, scene loading/unloading, and scene ordering
	class SceneElement
	{
	public:
		SceneElement(std::shared_ptr<Sol::Core> core);
		~SceneElement();

		//Initialize SceneElement
		ENGINE_API bool initialize();

		//Creates a new scene by resetting the ECS and the physics world
		void newScene();

		//Serialise the ECS Element's entity-component map into a JSON file, and reset the ECS's data structure(s) appropriately
		void unloadScene(const std::string& sceneFilepath);

		//Deserialise the JSON file into the ECS Element's entity-component map, and set the ECS's data structure(s) appropriately
		void loadScene(const std::string& sceneFilepath);

		const std::string& getCurrentScenePath() const
		{
			return m_currentScene;
		}

		//Terminate SceneElement
		void terminate();

	private:
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };

		std::string m_currentScene; //Current loaded scene in the Editor

		//Helper functions for serialisation of components
		json serializeTransform(const TransformComponent& component);
		json serializeInput(const InputComponent& component);
		json serializeCollider(const ColliderComponent& component);
		json serializePhysicsBody(const PhysicsBodyComponent& component);
		json serializeSprite(const SpriteComponent& component);

		//Helper functions for deserialisation of components
		TransformComponent deserializeTransform(const json& j);
		InputComponent deserializeInput(const json& j);
		ColliderComponent deserializeCollider(const json& j);
		PhysicsBodyComponent deserializePhysicsBody(const json& j);
		SpriteComponent deserializeSprite(const json& j);
	};
}