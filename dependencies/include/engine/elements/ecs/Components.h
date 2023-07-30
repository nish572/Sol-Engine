//Components.h

//Has the structs for all component types to be used by ECSElement
//Included in ECSElement.

#pragma once

#include <string>
#include "resource/ResourceElement.h"
#include <glm/glm.hpp>
#include <box2D/Box2D.h>

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


struct InputComponent
{
    glm::vec2 moveDirection;
    bool jump;
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
    Static,
    Dynamic,
    Kinematic
};

struct PhysicsBodyComponent {
    BodyType type;
    glm::vec2 linearVelocity;
    float angularVelocity;
    float linearDamping;
    float angularDamping;
    b2Body* body;

    PhysicsBodyComponent()
        : type(BodyType::Static), linearVelocity(0.0f), angularVelocity(0.0f), linearDamping(0.0f), angularDamping(0.0f), body(nullptr) {}

    PhysicsBodyComponent(BodyType rbType, const glm::vec2& rbLinearVelocity, float rbAngularVelocity, float rbLinearDamping, float rbAngularDamping, b2Body* rbBody)
        : type(rbType), linearVelocity(rbLinearVelocity), angularVelocity(rbAngularVelocity), linearDamping(rbLinearDamping), angularDamping(rbAngularDamping), body(rbBody) {}
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
