//------- Components --------
//The Component Definitions
//For The Sol Core Engine
//---------------------------

//Has the structs for all Component types to be used by ECSElement
//Included in ECSElement.

#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) //Disable several collider warnings - warnings considered, collider fully functional and trusted, no need for these warnings

#include <string>

#include "resource/ResourceElement.h"

//The external libraries GLM, Box2D, and SDL2
#include <glm/glm.hpp>
#include <box2D/Box2D.h>
#include <SDL.h>
#pragma warning(pop)

//Input type (either keyboard or mouse) to be saved to an action to be saved in the input component
enum class InputType
{
    Keyboard,
    MouseMovement
};
//Input type (options: force, impulse, torque, angular impulse) to be saved to an action to be saved in the input component
enum class ForceType
{
    Force,
    Impulse,
    Torque,
    AngularImpulse
};
//The action data to be stored in an input component
//Holds the direction a force should be applied (used by linear force or linear impulse), the magnitude (linear or angular), the force type, and the input type
struct ActionData {
    glm::vec2 moveDirection; //Direction of linear force or linear impulse
    float magnitude; //Amount of linear force or linear impulse
    float angularMagnitude; //Amount of torque or angular impulse, direction indicates clockwise or counter-clockwise
    ForceType fType;
    InputType iType;

    ActionData(glm::vec2 dir, float mag, float angMag, ForceType forceType, InputType inputType)
        : moveDirection(dir), magnitude(mag), angularMagnitude(angMag), fType(forceType), iType(inputType) {}
};
//Input Component
//Holds a map of actions where the key is the SDL Keycode, and the value is a vector of action data structures (which hold the data for that key press)
//Unlike other components, this component does include logic as well as data
//This was a workaround since normally components are purely data, but allows actions to be added/removed
//This still follows the ECS's design paradigm such that components should hold any game logic directly
struct InputComponent
{
    //Map from SDL_Keycode to a list of actions
    std::unordered_map<SDL_Keycode, std::vector<ActionData>> keyActions;

    //Add or remove key actions
    void addKeyAction(SDL_Keycode key, const ActionData& action) {
        keyActions[key].push_back(action);
    }
    void removeKeyAction(SDL_Keycode key) {
        keyActions.erase(key);
    }
    //Get all key actions
    std::vector<ActionData> getActionsForKey(SDL_Keycode key) {
        if (keyActions.find(key) != keyActions.end()) {
            return keyActions[key];
        }
        return {}; //Return empty vector if no actions found for the key
    }
};

//Transform Component
//Holds positional, rotational, and scalar data for a transform in the scene space
struct TransformComponent {
    glm::vec3 position;
    float rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(0.0f), scale(1.0f) {}

    TransformComponent(const glm::vec3& pos, float rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
};

//Collider's shape type (either a Box, or a Circle)
enum class ShapeType {
    Box,
    Circle,
};
//Collider Component
//Holds the visibiility, shape type, pointer to the Box2D shape, the dimensions of the collider, and several physics properties for the collider
struct ColliderComponent {
    bool isVisible; //Used to determine whether a wireframe should be visible in the playtest window, useful for editing and debugging an Entity
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
        : isVisible(false), shapeType(ShapeType::Box), width(1.0f), height(1.0f), radius(0.5f),
        density(1.0f), friction(0.3f), restitution(0.1f) {}

    //Constructor for box shape
    ColliderComponent(float boxWidth, float boxHeight, float boxDensity,
        float boxFriction, float boxRestitution)
        : isVisible(false), shapeType(ShapeType::Box), width(boxWidth), height(boxHeight), radius(0),
        density(boxDensity), friction(boxFriction), restitution(boxRestitution) {}

    //Constructor for circle shape
    ColliderComponent(float circleRadius, float circleDensity,
        float circleFriction, float circleRestitution)
        : isVisible(false), shapeType(ShapeType::Circle), width(0), height(0), radius(circleRadius),
        density(circleDensity), friction(circleFriction), restitution(circleRestitution) {}
};

//Bodies shape (either Dynamic, Static, or Kinematic)
enum class BodyType {
    Dynamic,
    Static,
    Kinematic
};
//Physics Body Component
//Holds the body type, position in the Box2D world, and the associated Box2D body (which is created based upon collider component data)
struct PhysicsBodyComponent {
    BodyType type;
    b2Vec2 position;
    b2Body* body;
    bool rotationLock;

    PhysicsBodyComponent()
        : type(BodyType::Static), position(0,0), body(nullptr), rotationLock(false) {}

    PhysicsBodyComponent(BodyType rbType, b2Vec2 pos, b2Body* rbBody, bool rotatable)
        : type(rbType), position(pos), body(rbBody), rotationLock(rotatable) {}
};

//Sprite Component
//Holds the texture ID to be used by rendering, a texture filepath which is useful when loading/unloading a scene, size and colour, and a shader program ID (currently unusued but useful for the furthering of the Shader Element)
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