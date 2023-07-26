//#include "systems/PhysicsSystem.h"
//#include "ecs/EcsElement.h"
//
//namespace EcsPhysicsSystem
//{
//    PhysicsSystem::PhysicsSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
//        : m_ecsElement(ecsElement)
//    {
//        b2Vec2 gravity(0.0f, -9.81f);
//        m_world = std::make_unique<b2World>(gravity);
//    }
//
//    PhysicsSystem::~PhysicsSystem()
//    {
//    }
//
//    bool PhysicsSystem::initialize()
//    {
//        return true;
//    }
//
//    void PhysicsSystem::update(double deltaTime)
//    {
//        //Not necessary for the PhysicsSystem as it doesn't require frame-dependent updates
//    }
//
//    void PhysicsSystem::fixedUpdate(double fixedTimestep)
//    {
//        float timeStep = static_cast<float>(fixedTimestep);
//        // Update the physics simulation
//        const int32 velocityIterations = 8;
//        const int32 positionIterations = 3;
//        m_world->Step(fixedTimestep, velocityIterations, positionIterations);
//
//        // Get components of the PhysicsBodyComponent and TransformComponent types
//        auto physicsComponents = m_ecsElement->getAllComponentsOfType<PhysicsBodyComponent>();
//        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
//
//        // Iterate through entities with PhysicsBodyComponent and TransformComponent
//        for (const auto& physicsPair : physicsComponents) {
//            Entity entity = physicsPair.first;
//            auto physicsComponent = physicsPair.second;
//
//            auto it = std::find_if(transformComponents.begin(), transformComponents.end(),
//                [&](const auto& pair) { return pair.first == entity; });
//            if (it == transformComponents.end()) {
//                continue;
//            }
//
//            auto transformComponent = it->second;
//
//            // Sync the physics body's position and rotation with the TransformComponent
//            b2Vec2 position = physicsComponent->body->GetPosition();
//            float angle = physicsComponent->body->GetAngle();
//            transformComponent->position.x = position.x;
//            transformComponent->position.y = position.y;
//            transformComponent->rotation = angle;
//
//            // Apply forces/impulses/torque based on InputComponent
//            if (ecsElementPtr->hasComponent<InputComponent>(entity)) {
//                auto inputComponent = ecsElementPtr->getComponent<InputComponent>(entity);
//                // Process input and apply forces/impulses/torque to the physics body
//                // Example: Apply force based on input direction
//                b2Vec2 force(inputComponent.moveDirection.x * physicsComponent->forceFactor,
//                    inputComponent.moveDirection.y * physicsComponent->forceFactor);
//                physicsComponent->body->ApplyForceToCenter(force, true);
//            }
//        }
//    }
//}