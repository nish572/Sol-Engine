#include "systems/PhysicsSystem.h"
#include "ecs/EcsElement.h"

namespace EcsPhysicsSystem
{
    PhysicsSystem::PhysicsSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
        : m_ecsElement(ecsElement)
    {
        initialize();
    }

    PhysicsSystem::~PhysicsSystem()
    {
    }

    void PhysicsSystem::initialize()
    {
        // Define the gravity vector
        b2Vec2 gravity(0.0f, -9.8f); // Earth's gravity in the negative y direction

        // Create the world with the specified gravity
        m_world = std::make_shared<b2World>(gravity);

        //Create body definitions
        b2BodyDef dynamicBodyDef;
        dynamicBodyDef.type = b2_dynamicBody;
        m_bodyDefs[BodyType::Dynamic] = dynamicBodyDef;

        b2BodyDef staticBodyDef;
        staticBodyDef.type = b2_staticBody; 
        m_bodyDefs[BodyType::Static] = staticBodyDef;

        b2BodyDef kinematicBodyDef;
        kinematicBodyDef.type = b2_kinematicBody;
        m_bodyDefs[BodyType::Kinematic] = kinematicBodyDef;
    }

    void PhysicsSystem::update(double deltaTime)
    {
        //Not necessary for the PhysicsSystem as it doesn't require frame-dependent updates
    }

    void PhysicsSystem::fixedUpdate(double fixedTimestep)
    {
        const int32 velocityIterations = 8;
        const int32 positionIterations = 3;

        float timeStep = static_cast<float>(fixedTimestep);
        m_world->Step(fixedTimestep, velocityIterations, positionIterations);

        //Get components of the PhysicsBodyComponent and TransformComponent types
        auto physicsComponents = m_ecsElement->getAllComponentsOfType<PhysicsBodyComponent>();
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto colliderComponents = m_ecsElement->getAllComponentsOfType<ColliderComponent>();

        //Iterate through entities with PhysicsBodyComponent and TransformComponent
        for (const auto& physicsPair : physicsComponents) {
            Entity entity = physicsPair.first;
            auto& physicsComponent = physicsPair.second;

            //If the body has not been created yet, create it.
            if (physicsComponent->body == nullptr) {
                b2BodyDef& bodyDef = m_bodyDefs[physicsComponent->type];
                std::cout << bodyDef.type << std::endl;
                physicsComponent->body = m_world->CreateBody(&bodyDef);
                //Set the new position of the body
                b2Vec2 newPosition(physicsComponent->position.x, physicsComponent->position.y);
                //physicsComponent->body->SetTransform(newPosition, physicsComponent->body->GetAngle());
            }

            //Check for the ColliderComponent of the entity
            auto colliderIt = colliderComponents.find(entity);
            if (colliderIt != colliderComponents.end()) {
                auto& colliderComponent = colliderIt->second;
                if (colliderComponent->shape != nullptr) { //Ensure there's a shape defined
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = colliderComponent->shape;
                    fixtureDef.density = colliderComponent->density;
                    fixtureDef.friction = colliderComponent->friction;
                    fixtureDef.restitution = colliderComponent->restitution;

                    physicsComponent->body->CreateFixture(&fixtureDef);
                }
                //Err
            }

            auto transformIt = transformComponents.find(entity);
            if (transformIt == transformComponents.end()) {
                continue;
            }

            auto& transformComponent = transformIt->second; //potential error, reorder

            //Sync the physics body's position and rotation with the TransformComponent
            b2Vec2 pos = physicsComponent->body->GetPosition();
            //float angle = physicsComponent->body->GetAngle();
            transformComponent->position.x = pos.x;
            transformComponent->position.y = pos.y;
            //transformComponent->rotation = angle;
            std::cout << pos.y << std::endl;
        }

        //Apply forces/impulses/torque based on InputComponent
        //...
    }
}