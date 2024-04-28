//------- Physics System ----
//Implements Physics Simulations
//For The Sol Core Engine
//---------------------------

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
        //Get the world with the specified gravity
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
        //Not necessary for the Physics System as it doesn't require frame-dependent updates
    }

    void PhysicsSystem::fixedUpdate(double fixedTimestep)
    {
        const int32 velocityIterations = 8;
        const int32 positionIterations = 3;

        float timeStep = static_cast<float>(fixedTimestep);

        //Get Components of the PhysicsBodyComponent and TransformComponent types
        auto physicsComponents = m_ecsElement->getAllComponentsOfType<PhysicsBodyComponent>();
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto colliderComponents = m_ecsElement->getAllComponentsOfType<ColliderComponent>();
        
        //Retrieve actions for this frame
        auto& actionsPerEntity = m_ecsElement->getCore()->getEventElement()->getActionsForPhysics();

        //Iterate through Entities with PhysicsBodyComponent and TransformComponent
        //This is to ensure all bodies are created with the appropriate fixtures and at the appropriate positions
        for (const auto& physicsPair : physicsComponents) {
            Entity entity = physicsPair.first;
            auto& physicsComponent = physicsPair.second;

            auto transformIt = transformComponents.find(entity);

            if (transformIt == transformComponents.end()) {
                continue;
            }

            auto& transformComponent = transformIt->second;

            //Determine the initial body positions based upon the transform component's positions
            //Special care taken to offset due to the Box2D defining an origin in a different location to the OpenGL space
            //Special care taken to also scale the positional data for the scene world space since Box2D uses meters, not pixels
            physicsComponent->position.x = ((transformComponent->position.x) - (ApplicationConfig::Config::screenWidth / 2.0f)) / m_scalingFactor;
            physicsComponent->position.y = ((transformComponent->position.y) - (ApplicationConfig::Config::screenHeight / 2.0f)) / m_scalingFactor;

            //Determine the initial body rotation based upon the transform component's rotational data
            //Convert rotation from degrees to radians
            //This is because Box2D uses radians, however the transform component uses degrees
            float newAngle = transformComponent->rotation * b2_pi / 180.0f;

            //If the body has not been created yet, create it
            if (physicsComponent->body == nullptr) {
                if (physicsComponent->type == BodyType::Static)
                {
                    b2BodyDef& bodyDef = m_bodyDefs[BodyType::Static];
                    physicsComponent->body = m_world->CreateBody(&bodyDef);
                    physicsComponent->body->SetFixedRotation(physicsComponent->rotationLock);
                }
                if (physicsComponent->type == BodyType::Dynamic)
                {
                    b2BodyDef& bodyDef = m_bodyDefs[BodyType::Dynamic];
                    physicsComponent->body = m_world->CreateBody(&bodyDef);
                    physicsComponent->body->SetFixedRotation(physicsComponent->rotationLock);
                }
                if (physicsComponent->type == BodyType::Kinematic)
                {
                    //In this implementation, setting a kinematic body actually sets a Box2D dynamic body
                    //Box2D does have a kinematic body, however this does not collide with static or dynamic bodies, which is undesired in this implementation
                    b2BodyDef kinematicBodyDef;
                    kinematicBodyDef.type = b2_dynamicBody;
                    kinematicBodyDef.gravityScale = 0.0f; //Set gravity to 0 so the user can produce top-down scenes for example where movement is non-human e.g. the game Asteroids

                    kinematicBodyDef.position = m_bodyDefs[BodyType::Dynamic].position;
                    kinematicBodyDef.angle = m_bodyDefs[BodyType::Dynamic].angle;

                    physicsComponent->body = m_world->CreateBody(&kinematicBodyDef);
                    physicsComponent->body->SetFixedRotation(physicsComponent->rotationLock);
                }

                //Set the new position and rotation of the body based upon the transform component's data that was pre-determined
                b2Vec2 newPosition(physicsComponent->position.x, physicsComponent->position.y);
                physicsComponent->body->SetTransform(newPosition, newAngle);

                //Check for the ColliderComponent of the entity
                auto colliderIt = colliderComponents.find(entity);
                if (colliderIt != colliderComponents.end()) {
                    auto& colliderComponent = colliderIt->second;
                    //Set the fixture's density, friction, and restitution
                    b2FixtureDef fixtureDef;
                    fixtureDef.density = colliderComponent->density;
                    fixtureDef.friction = colliderComponent->friction;
                    fixtureDef.restitution = colliderComponent->restitution;

                    //Set the Box2D shape based upon the collider component's type
                    switch (colliderComponent->shapeType) {
                    case ShapeType::Box:
                    {
                        b2PolygonShape boxShape;
                        boxShape.SetAsBox(colliderComponent->width / 2.0f, colliderComponent->height / 2.0f); //Box2D uses half-widths and half-heights, whereas the collider component uses full widths and heights, so halve here
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

            //Find and apply input actions specific to this entity
            auto actionsIt = actionsPerEntity.find(entity);
            if (actionsIt != actionsPerEntity.end()) {
                const auto& actions = actionsIt->second;
                for (const ActionData& action : actions) {
                    //Apply each action
                    b2Vec2 moveDirection(action.moveDirection.x, action.moveDirection.y);
                    moveDirection.Normalize();
                    float magnitude = action.magnitude;
                    float angularMagnitude = action.angularMagnitude;
                    //Switch based on the type of force to apply
                    switch (action.fType) {
                    //Force is a linear force applied to the centre of mass   
                    case ForceType::Force:
                        physicsComponent->body->ApplyForceToCenter(magnitude * moveDirection, true);
                        break;
                    //Impulse is a linear impulse applied to the centre of mass
                    case ForceType::Impulse:
                        physicsComponent->body->ApplyLinearImpulseToCenter(magnitude * moveDirection, true);
                        break;
                    //Torque is an angular force applied with origin of rotation as the centre of mass
                    case ForceType::Torque:
                        physicsComponent->body->ApplyTorque(angularMagnitude, true);
                        break;
                    //Angular impulse is an angular impulse applied with origin of rotation as the centre of mass
                    case ForceType::AngularImpulse:
                        physicsComponent->body->ApplyAngularImpulse(angularMagnitude, true);
                        break;
                    }
                }
            }

            //Sync the physics body's position and rotation with the TransformComponent
            //This enables the transform component to be used in rendering to accurately render the position and rotation of the Entity (if a sprite component is present, which it almost always will be)
            b2Vec2 pos = physicsComponent->body->GetPosition();
            float angle = physicsComponent->body->GetAngle();
            //Special care taken to offset due to the Box2D defining an origin in a different location to the OpenGL space
            //Special care taken to also scale the positional data for the scene world space since Box2D uses meters, not pixels
            transformComponent->position.x = (pos.x * m_scalingFactor) + (ApplicationConfig::Config::screenWidth / 2.0f); 
            transformComponent->position.y = (pos.y * m_scalingFactor) + (ApplicationConfig::Config::screenHeight / 2.0f);
            transformComponent->rotation = angle * 180.0f / b2_pi; //Conversion back to degrees
        }

        //Step the physics world, which is a Box2D design paradigm to signal the simulation of physics and preparation of the next step
        m_world->Step(fixedTimestep, velocityIterations, positionIterations);
    }
}