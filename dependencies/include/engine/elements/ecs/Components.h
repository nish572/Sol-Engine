//Components.h

//Has the structs for all component types to be used by ECSElement
//Included in ECSElement.

#pragma once

#include <string>
#include "resource/ResourceElement.h"
#include <glm/glm.hpp>
#include <box2D/Box2D.h>
#include <SDL.h>

//Either I upgrade to c++17 to use below, or use boost::any, however boost is rather large
//struct ScriptableComponent
//{
//    std::unordered_map<std::string, std::any> fields;
//
//    void setField(const std::string& name, const std::any& value) {
//        fields[name] = value;
//    }
//
//    std::any getField(const std::string& name) {
//        return fields[name];
//    }
//};

enum class InputType
{
    Keyboard,
    MouseMovement
};

enum class ForceType
{
    Force,
    Impulse
};

struct ActionData {
    glm::vec2 moveDirection; //Direction of force
    float magnitude; //Amount of force
    ForceType fType;
    InputType iType;

    ActionData(glm::vec2 dir, float mag, ForceType forceType, InputType inputType)
        : moveDirection(dir), magnitude(mag), fType(forceType), iType(inputType) {}
};

struct InputComponent
{
    // Map from SDL_Keycode to a list of actions
    std::unordered_map<SDL_Keycode, std::vector<ActionData>> keyActions;

    void addKeyAction(SDL_Keycode key, const ActionData& action) {
        keyActions[key].push_back(action);
    }
    void removeKeyAction(SDL_Keycode key) {
        // Example: Removing all actions for a key
        keyActions.erase(key);
    }
    std::vector<ActionData> getActionsForKey(SDL_Keycode key) {
        if (keyActions.find(key) != keyActions.end()) {
            return keyActions[key];
        }
        return {}; // Return empty vector if no actions found for the key
    }
};

struct TransformComponent {
    glm::vec3 position;
    float rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(0.0f), scale(1.0f) {}

    TransformComponent(const glm::vec3& pos, float rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
};

struct ColliderComponent {
    b2Shape* shape;
    float density;
    float friction;
    float restitution;

    ColliderComponent()
        : shape(nullptr), density(1.0f), friction(0.3f), restitution(0.1f) {}

    ColliderComponent(b2Shape* colliderShape, float colliderDensity, float colliderFriction, float colliderRestitution)
        : shape(colliderShape), density(colliderDensity), friction(colliderFriction), restitution(colliderRestitution) {}
};

enum class BodyType {
    Dynamic,
    Static,
    Kinematic
};

struct PhysicsBodyComponent {
    BodyType type;
    b2Vec2 position;
    b2Body* body;

    PhysicsBodyComponent()
        : type(BodyType::Static), position(0,0), body(nullptr) {}

    PhysicsBodyComponent(BodyType rbType, b2Vec2 pos, b2Body* rbBody)
        : type(rbType), position(pos), body(rbBody) {}
};

struct SpriteComponent {
    unsigned int textureID;
    glm::vec2 size;
    glm::vec4 color;
    unsigned int shaderProgram;

    SpriteComponent()
        : textureID(0), size(1.0f), color(1.0f), shaderProgram(0) {}

    SpriteComponent(unsigned int texID, const glm::vec2& spriteSize, const glm::vec4& spriteColor, unsigned int programID)
        : textureID(texID), size(spriteSize), color(spriteColor), shaderProgram(programID) {}
};
