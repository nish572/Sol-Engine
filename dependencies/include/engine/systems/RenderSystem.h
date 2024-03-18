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

namespace Sol {
    class Core;
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

        unsigned int createShader(const char* vertexShaderSource, const char* fragmentShaderSource);

        void renderSprites(std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs);

        void singleDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch);
        void multiDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch);

    private:
        //Pointer to EcsElement
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement;
        //Pointer to Core
        std::weak_ptr<Sol::Core> m_core;

        //Main VAO, quad VBO/EBO, and model matrices' VBO
        unsigned int m_mainVAO;
        unsigned int m_quadVBO;
        unsigned int m_quadEBO;
        unsigned int m_modelVBO;

        //Shader program ID's for default rendering of a sprite
        //One for if the draw call is per sprite, one for if the draw call is for a batch to be drawn via instancing
        unsigned int m_defaultShaderID;
        unsigned int m_defaultInstanceShaderID;

        //Max number of spites that can be drawn total
        const int m_MAX_SPRITES = 100000;

        //Cache for the calculated modelMatrices
        std::vector<glm::mat4> m_modelMatricesCache;

        //Minimum number of sprites required for instanced rendering to be used instead of individual draw calls
        const unsigned int m_MIN_SPRITES_FOR_INSTANCING = 100;
    };
}
