#pragma once

#include <memory>

#include <glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ecs/Components.h"

using Entity = std::uint32_t;

//Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
}

struct SpriteComponent;

namespace EcsRenderSystem
{
    class RenderSystem
    {
    public:
        RenderSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        ~RenderSystem();

        void initialize();

        void update(double deltaTime);

        void fixedUpdate(double fixedTimestep);

        void renderSprite(std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<SpriteComponent> spriteComponent);

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;
        unsigned int m_sharedVAO;
        unsigned int m_sharedVBO;
        unsigned int m_sharedEBO;
    };
}
