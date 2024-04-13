#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "ecs/Components.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
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
	//Represents the element responsible for managing the scene operations in the Sol Engine
	//This includes serialisation/deserialisation, scene loading/unloading, and scene ordering
	class SceneElement
	{
	public:
		//Initialize pointers
		SceneElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the SceneElement instance
		~SceneElement();

		//Initialize SceneElement
		ENGINE_API bool initialize();

		//Serialize the ECS Element's entity-component map into a JSON file], and set the ECS's data structure(s) appropriately
		void serializeScene(const std::string& sceneFilepath);

		//Deserialize the JSON file into the ECS Element's entity-component map, and reset the ECS's data structure(s) appropriately
		void deserializeScene(const std::string& sceneFilepath);

		//Load a scene by calling deserialize function
		void loadScene(const std::string& sceneFilepath);

		//Unoad a scene by calling serialize function
		void unloadScene(const std::string& sceneFilepath);

		//Terminate SceneElement
		void terminate();

	private:
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };

		// Helper functions for serialization of components
		json serializeTransform(const TransformComponent& component);
		json serializeInput(const InputComponent& component);
		json serializeCollider(const ColliderComponent& component);
		json serializePhysicsBody(const PhysicsBodyComponent& component);
		json serializeSprite(const SpriteComponent& component);

		// Helper functions for deserialization of components
		TransformComponent deserializeTransform(const json& j);
		InputComponent deserializeInput(const json& j);
		ColliderComponent deserializeCollider(const json& j);
		PhysicsBodyComponent deserializePhysicsBody(const json& j);
		SpriteComponent deserializeSprite(const json& j);
	};
}