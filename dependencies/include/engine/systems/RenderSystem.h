#pragma once

#include <memory>
#include <glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declarations to avoid circular dependencies
#pragma once

#include <memory>
#include <glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
    struct SpriteComponent;
}

namespace EcsRenderSystem
{
    class RenderSystem
    {
    public:
        RenderSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement);

        void update(double deltaTime);
        void addSpriteComponent(Entity entity, std::shared_ptr<CoreEcsElement::SpriteComponent> spriteComponent, GLuint textureID);
        void renderSprite(std::shared_ptr<CoreEcsElement::SpriteComponent> spriteComponent);

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;
        ;
        GLuint m_spriteVAO;
        GLuint m_spriteEBO;
        GLuint m_spriteVBO;
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
    };
}
