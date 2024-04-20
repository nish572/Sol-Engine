#include "systems/PhysicsSystem.h"
#include "ecs/EcsElement.h"

#include "Core.h"

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
        //Create the world with the specified gravity
        m_world = m_ecsElement->getCore()->getPhysicsElement()->getWorld();

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

        //Get components of the PhysicsBodyComponent and TransformComponent types
        auto physicsComponents = m_ecsElement->getAllComponentsOfType<PhysicsBodyComponent>();
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto colliderComponents = m_ecsElement->getAllComponentsOfType<ColliderComponent>();
        
        // Retrieve actions for this frame
        auto& actionsPerEntity = m_ecsElement->getCore()->getEventElement()->getActionsForPhysics();

        //Iterate through entities with PhysicsBodyComponent and TransformComponent
        //This is to ensure all bodies are created with the appropriate fixtures and at the appropriate positions
        for (const auto& physicsPair : physicsComponents) {
            Entity entity = physicsPair.first;
            auto& physicsComponent = physicsPair.second;

            auto transformIt = transformComponents.find(entity);

            if (transformIt == transformComponents.end()) {
                continue;
            }

            auto& transformComponent = transformIt->second; //potential error, reorder

            physicsComponent->position.x = ((transformComponent->position.x) - (ApplicationConfig::Config::screenWidth / 2.0f)) / m_scalingFactor;
            physicsComponent->position.y = ((transformComponent->position.y) - (ApplicationConfig::Config::screenHeight / 2.0f)) / m_scalingFactor;

            //Convert rotation from degrees to radians
            float newAngle = transformComponent->rotation * b2_pi / 180.0f;
            std::cout << transformComponent->rotation << " And " << newAngle << std::endl;

            //If the body has not been created yet, create it.
            if (physicsComponent->body == nullptr) {
                b2BodyDef& bodyDef = m_bodyDefs[physicsComponent->type];
                physicsComponent->body = m_world->CreateBody(&bodyDef);
                //Set the new position of the body
                b2Vec2 newPosition(physicsComponent->position.x, physicsComponent->position.y);
                physicsComponent->body->SetTransform(newPosition, newAngle);

                //Check for the ColliderComponent of the entity
                auto colliderIt = colliderComponents.find(entity);
                if (colliderIt != colliderComponents.end()) {
                    auto& colliderComponent = colliderIt->second;

                    b2FixtureDef fixtureDef;
                    fixtureDef.density = colliderComponent->density;
                    fixtureDef.friction = colliderComponent->friction;
                    fixtureDef.restitution = colliderComponent->restitution;

                    switch (colliderComponent->shapeType) {
                    case ShapeType::Box:
                    {
                        b2PolygonShape boxShape;
                        boxShape.SetAsBox(colliderComponent->width / 2.0f, colliderComponent->height / 2.0f); //Box2D uses half-widths and half-heights
                        fixtureDef.shape = &boxShape;
                        physicsComponent->body->CreateFixture(&fixtureDef);
                    }
                    break;
                    case ShapeType::Circle:
                    {
                        b2CircleShape circleShape;
                        circleShape.m_radius = colliderComponent->radius;
                        fixtureDef.shape = &circleShape;
                        physicsComponent->body->CreateFixture(&fixtureDef);
                    }
                    break;
                    }
                }
            }

            //Find and apply actions specific to this entity
            auto actionsIt = actionsPerEntity.find(entity);
            if (actionsIt != actionsPerEntity.end()) {
                const auto& actions = actionsIt->second;
                for (const ActionData& action : actions) {
                    //Apply each action
                    b2Vec2 moveDirection(action.moveDirection.x, action.moveDirection.y);
                    moveDirection.Normalize();
                    float magnitude = action.magnitude;
                    //Switch based on the type of force to apply
                    switch (action.fType) {
                    case ForceType::Force:
                        physicsComponent->body->ApplyForceToCenter(magnitude * moveDirection, true);
                        break;
                    case ForceType::Impulse:
                        physicsComponent->body->ApplyLinearImpulseToCenter(magnitude * moveDirection, true);
                        break;
                        //Handle other force types as needed
                    }
                }
            }

            //Sync the physics body's position and rotation with the TransformComponent
            b2Vec2 pos = physicsComponent->body->GetPosition();
            float angle = physicsComponent->body->GetAngle();
            transformComponent->position.x = (pos.x * m_scalingFactor) + (ApplicationConfig::Config::screenWidth / 2.0f);
            transformComponent->position.y = (pos.y * m_scalingFactor) + (ApplicationConfig::Config::screenHeight / 2.0f);
            transformComponent->rotation = angle * 180.0f / b2_pi;
        }

        //Physics step
        m_world->Step(fixedTimestep, velocityIterations, positionIterations);
    }
}