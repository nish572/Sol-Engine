//Components.h

//Has the structs for all component types to be used by ECSElement
//Included in ECSElement.

#pragma once

#include <string>
#include "resource/ResourceElement.h"
#include <glm/glm.hpp>

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(0.0f), scale(1.0f) {}

    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
};

struct ColliderComponent {
    glm::vec3 size;

    ColliderComponent()
        : size(1.0f) {}

    ColliderComponent(const glm::vec3& colliderSize)
        : size(colliderSize) {}
};

enum class RigidBodyType {
    Static,
    Dynamic,
    Kinematic
};

struct RigidBodyComponent {
    RigidBodyType type;
    float mass;

    RigidBodyComponent()
        : type(RigidBodyType::Static), mass(0.0f) {}

    RigidBodyComponent(RigidBodyType rbType, float rbMass)
        : type(rbType), mass(rbMass) {}
};

//struct ScriptableComponent {
//    std::unordered_map<std::string, boost::variant<bool, int, float, std::string>> customData;
//};

struct SpriteComponent {
    unsigned int textureID;
    glm::vec2 size;
    glm::vec4 color;

    unsigned int VAO;
    unsigned int instanceVBO;
    unsigned int shaderProgram; // Add this line

    SpriteComponent()
        : textureID(0), size(1.0f), color(1.0f), VAO(0), instanceVBO(0), shaderProgram(0) {} // Update this line

    SpriteComponent(unsigned int texID, const glm::vec2& spriteSize, const glm::vec4& spriteColor, unsigned int programID) // Update this line
        : textureID(texID), size(spriteSize), color(spriteColor), VAO(0), instanceVBO(0), shaderProgram(programID) {} // Update this line
};
