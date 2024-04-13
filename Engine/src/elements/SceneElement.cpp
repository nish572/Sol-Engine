#include "scene/SceneElement.h"

#include "Core.h"


namespace CoreSceneElement
{
	SceneElement::SceneElement(std::shared_ptr<Sol::Core> core) : m_core(core)
	{
	}
	SceneElement::~SceneElement()
	{
	}

	bool SceneElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}

		//Then log success and return true
		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Scene] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Scene] Successfully Initialized" << std::endl;
		return true;
	}

	//Serialization function for the scene

	void SceneElement::serializeScene(const std::string& sceneFilepath) {
		json j; //Create JSON object to hold all ECS data

		auto corePtr = m_core.lock();
		if (corePtr)
		{
			auto ecsPtr = corePtr->getEcsElement();
			if (ecsPtr)
			{
				const auto& entityMap = ecsPtr->getEntityMap();
				for (const auto& pair : entityMap) {
					Entity entity = pair.first;
					const auto& componentsMap = pair.second;
					json jEntity;

					//Serialize each component based on its type
					for (const auto& compPair : componentsMap) {
						const std::type_index& typeIndex = compPair.first;
						const auto& component = compPair.second;
						if (typeIndex == typeid(TransformComponent)) {
							jEntity["TransformComponent"] = serializeTransform(*std::static_pointer_cast<TransformComponent>(component));
						}
						else if (typeIndex == typeid(InputComponent)) {
							jEntity["InputComponent"] = serializeInput(*std::static_pointer_cast<InputComponent>(component));
						}
						else if (typeIndex == typeid(ColliderComponent)) {
							jEntity["ColliderComponent"] = serializeCollider(*std::static_pointer_cast<ColliderComponent>(component));
						}
						else if (typeIndex == typeid(PhysicsBodyComponent)) {
							jEntity["PhysicsBodyComponent"] = serializePhysicsBody(*std::static_pointer_cast<PhysicsBodyComponent>(component));
						}
						else if (typeIndex == typeid(SpriteComponent)) {
							jEntity["SpriteComponent"] = serializeSprite(*std::static_pointer_cast<SpriteComponent>(component));
						}
					}
					j[std::to_string(entity)] = jEntity;
				}
			}
		}
		//Write the JSON object to a file
		std::ofstream file(sceneFilepath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + sceneFilepath);
		}
		file << j.dump(4);  //Dump the JSON with an indentation of 4 spaces
		file.close();
	}

	//Deserialization function for the scene

	void SceneElement::deserializeScene(const std::string& sceneFilepath) {

		std::ifstream file(sceneFilepath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + sceneFilepath);
		}

		json j;
		file >> j;  //Load the JSON from the file

		auto corePtr = m_core.lock();
		if (corePtr)
		{
			auto ecsPtr = corePtr->getEcsElement();
			if (ecsPtr)
			{
				ecsPtr->clear();  //Prepare the ECS for new scene data
				for (const auto& entityPair : j.items()) {
					const std::string& entityIdStr = entityPair.key();
					const json& components = entityPair.value();

					Entity newEntity = ecsPtr->createEntity();
					for (const auto& componentPair : components.items()) {
						const std::string& compType = componentPair.key();
						const json& compJson = componentPair.value();

						if (compType == "TransformComponent") {
							TransformComponent tc = deserializeTransform(compJson);
							ecsPtr->addComponent(newEntity, tc);
						}
						else if (compType == "InputComponent") {
							InputComponent ic = deserializeInput(compJson);
							ecsPtr->addComponent(newEntity, ic);
						}
						else if (compType == "ColliderComponent") {
							ColliderComponent cc = deserializeCollider(compJson);
							ecsPtr->addComponent(newEntity, cc);
						}
						else if (compType == "PhysicsBodyComponent") {
							PhysicsBodyComponent pb = deserializePhysicsBody(compJson);
							ecsPtr->addComponent(newEntity, pb);
						}
						else if (compType == "SpriteComponent") {
							SpriteComponent sc = deserializeSprite(compJson);
							ecsPtr->addComponent(newEntity, sc);
						}
					}
				}
			}
		}
	}

	//Serialization functions for each component type

	json SceneElement::serializeTransform(const TransformComponent& component) {
		return {
			{"position", {component.position.x, component.position.y, component.position.z}},
			{"rotation", component.rotation},
			{"scale", {component.scale.x, component.scale.y, component.scale.z}}
		};
	}

	json SceneElement::serializeInput(const InputComponent& component) {
		json j;
		for (const auto& pair : component.keyActions) {
			SDL_Keycode key = pair.first;
			const std::vector<ActionData>& actions = pair.second;

			json jActions;
			for (const auto& action : actions) {
				jActions.push_back({
					{"moveDirection", {action.moveDirection.x, action.moveDirection.y}},
					{"magnitude", action.magnitude},
					{"forceType", static_cast<int>(action.fType)},
					{"inputType", static_cast<int>(action.iType)}
					});
			}
			j[std::to_string(key)] = jActions;
		}
		return j;
	}

	json SceneElement::serializeCollider(const ColliderComponent& component) {
		json shapeData;
		switch (component.shapeType) {
		case ShapeType::Box:
			shapeData = {
				{"type", "Box"},
				{"width", component.width},
				{"height", component.height}
			};
			break;
		case ShapeType::Circle:
			shapeData = {
				{"type", "Circle"},
				{"radius", component.radius}
			};
			break;
		}

		return {
			{"shape", shapeData},
			{"density", component.density},
			{"friction", component.friction},
			{"restitution", component.restitution}
		};
	}

	json SceneElement::serializePhysicsBody(const PhysicsBodyComponent& component) {
		return {
			{"type", static_cast<int>(component.type)},
			{"position", {component.position.x, component.position.y}}
			//Body will be recreated based on the scene setup, not serialized
		};
	}

	json SceneElement::serializeSprite(const SpriteComponent& component) {
		return {
			{"textureFilePath", component.textureFilePath},
			{"size", {component.size.x, component.size.y}},
			{"color", {component.color.r, component.color.g, component.color.b, component.color.a}},
			{"shaderProgram", component.shaderProgram} //Redundant since nowhere we are actually setting this value for the sprite, just here as placeholder for now
		};
	}

	//Deserialization functions for each component type

	TransformComponent SceneElement::deserializeTransform(const json& j) {
		glm::vec3 position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
		float rotation = j["rotation"];
		glm::vec3 scale = glm::vec3(j["scale"][0], j["scale"][1], j["scale"][2]);
		return TransformComponent(position, rotation, scale);
	}

	InputComponent SceneElement::deserializeInput(const json& j) {
		InputComponent inputComponent;
		for (auto& item : j.items()) {
			SDL_Keycode key = std::stoi(item.key());
			std::vector<ActionData> actions;
			for (auto& action : item.value()) {
				glm::vec2 moveDirection = glm::vec2(action["moveDirection"][0], action["moveDirection"][1]);
				float magnitude = action["magnitude"];
				ForceType fType = static_cast<ForceType>(action["forceType"]);
				InputType iType = static_cast<InputType>(action["inputType"]);
				actions.emplace_back(moveDirection, magnitude, fType, iType);
			}
			inputComponent.keyActions[key] = actions;
		}
		return inputComponent;
	}

	ColliderComponent SceneElement::deserializeCollider(const json& j) {
		float density = j["density"];
		float friction = j["friction"];
		float restitution = j["restitution"];
		json shapeData = j["shape"];

		ShapeType shapeType;
		float width = 0, height = 0, radius = 0;

		std::string type = shapeData["type"];
		if (type == "Box") {
			shapeType = ShapeType::Box;
			width = shapeData["width"];
			height = shapeData["height"];
		}
		else if (type == "Circle") {
			shapeType = ShapeType::Circle;
			radius = shapeData["radius"];
		}

		ColliderComponent colliderComponent;
		colliderComponent.shapeType = shapeType;
		colliderComponent.width = width;
		colliderComponent.height = height;
		colliderComponent.radius = radius;
		colliderComponent.density = density;
		colliderComponent.friction = friction;
		colliderComponent.restitution = restitution;

		return colliderComponent;
	}

	PhysicsBodyComponent SceneElement::deserializePhysicsBody(const json& j) {
		BodyType type = static_cast<BodyType>(j["type"]);
		b2Vec2 position = b2Vec2(j["position"][0], j["position"][1]);
		//Body will be reconstructed during the physics world setup, not directly from JSON
		return PhysicsBodyComponent(type, position, nullptr);
	}

	SpriteComponent SceneElement::deserializeSprite(const json& j) {
		std::string texturePath = j["textureFilePath"];
		glm::vec2 size = glm::vec2(j["size"][0], j["size"][1]);
		glm::vec4 color = glm::vec4(j["color"][0], j["color"][1], j["color"][2], j["color"][3]);
		unsigned int shaderProgram = j["shaderProgram"];

		auto corePtr = m_core.lock();
		if (corePtr) {
			std::shared_ptr<TextureResource> textureResource = corePtr->getResourceElement()->loadTextureResource(texturePath);
			unsigned int textureID = textureResource ? textureResource->textureID : 0;

			//Adjust size based on texture dimensions
			if (textureResource) {
				size = glm::vec2(textureResource->width, textureResource->height);
			}

			return SpriteComponent(textureID, texturePath, size, color, shaderProgram);
		}
	}

	void SceneElement::terminate()
	{
		return;
	}
}