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
    //Map from SDL_Keycode to a list of actions
    std::unordered_map<SDL_Keycode, std::vector<ActionData>> keyActions;

    void addKeyAction(SDL_Keycode key, const ActionData& action) {
        keyActions[key].push_back(action);
    }
    void removeKeyAction(SDL_Keycode key) {
        keyActions.erase(key);
    }
    std::vector<ActionData> getActionsForKey(SDL_Keycode key) {
        if (keyActions.find(key) != keyActions.end()) {
            return keyActions[key];
        }
        return {}; //Return empty vector if no actions found for the key
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

enum class ShapeType {
    Box,
    Circle,
    //Expand with other shapes if needed
};

struct ColliderComponent {
    ShapeType shapeType;
    b2Shape* shape;
    float width;  //Used for the width of a box
    float height; //Used for the height of a box
    float radius; //Used for the radius of a circle

    float density;
    float friction;
    float restitution;

    //Default constructor initializes to a default box shape as a placeholder
    ColliderComponent()
        : shapeType(ShapeType::Box), width(1.0f), height(1.0f), radius(0.5f),
        density(1.0f), friction(0.3f), restitution(0.1f) {}

    //Constructor for box shape
    ColliderComponent(float boxWidth, float boxHeight, float boxDensity,
        float boxFriction, float boxRestitution)
        : shapeType(ShapeType::Box), width(boxWidth), height(boxHeight), radius(0),
        density(boxDensity), friction(boxFriction), restitution(boxRestitution) {}

    //Constructor for circle shape
    ColliderComponent(float circleRadius, float circleDensity,
        float circleFriction, float circleRestitution)
        : shapeType(ShapeType::Circle), width(0), height(0), radius(circleRadius),
        density(circleDensity), friction(circleFriction), restitution(circleRestitution) {}
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
    std::string textureFilePath;  //Path to texture for serialization/deserialization
    glm::vec2 size;
    glm::vec4 color;
    unsigned int shaderProgram;

    SpriteComponent()
        : textureID(0), textureFilePath(""), size(1.0f), color(1.0f), shaderProgram(0) {}

    SpriteComponent(unsigned int texID, const std::string& filePath, const glm::vec2& spriteSize,
        const glm::vec4& spriteColor, unsigned int programID)
        : textureID(texID), textureFilePath(filePath), size(spriteSize),
        color(spriteColor), shaderProgram(programID) {}
};