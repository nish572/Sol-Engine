#pragma once

#pragma warning(push)
#pragma warning(disable: 4018 4244) 

//C++ libraries
#include <memory>

//External libraries GLAD and GLM
#include <glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ecs/Components.h"
#pragma warning(pop)

//The definition of an Entity (i.e. an Entity exists as an integer)
using Entity = std::uint32_t;

//Forward declarations to avoid circular dependencies
namespace CoreEcsElement {
    class EcsElement;
}

namespace Sol {
    class Core;
}

namespace ApplicationConfig
{
    class Config;
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

        //Update is used since rendering is dependent upon a variable delta timesep
        void update(double deltaTime);

        //Fixed update is not used since rendering is dependent upon a variable delta timesep
        void fixedUpdate(double fixedTimestep);

        //A function to create the default shader
        //There is a Shader Element, however this isn't used since a default shader is preferable in-code as opposed to loading from two separate viles
        unsigned int createShader(const char* vertexShaderSource, const char* fragmentShaderSource);

        //Determines whether single draw or multi draw should be called 
        void renderSprites(std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs);

        //Single drawing will be used if the number of sprites to be rendered with the same texture is less than the minimum number of sprites required to make batch (instanced) rendering worthwile
        void singleDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch);
        //Multi drawing is in essence batch (instanced) rendering, so renders all sprites with the same texture in one draw call if there are enough sprites to make this worthwhile
        void multiDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch);

        //To render box or circle wireframes of the collider component (made to be only possible through the Editor)
        void renderWireframe(const TransformComponent& transform, const ColliderComponent& collider, bool isCircle);

        //Auxilliary function to set the projection matrix, useful in case the aspect ratio changes (i.e. if the window changes size)
        void setProjectionMatrix();

    private:
        std::shared_ptr<CoreEcsElement::EcsElement> m_ecsElement; //Shared pointer of the ECS Element to allow the Render System to manage the ECS's lifetime otherwise the ECS may terminate before the rendering is finished
        //Pointer to Core
        std::weak_ptr<Sol::Core> m_core;

        //VAO for managing VBOs
        //VBO as data container for vertex or instance attributes
        //EBO for indices to process vertices

        //Main VAO, quad VBO/EBO, and model matrices' VBO
        unsigned int m_mainVAO;
        unsigned int m_quadVBO;
        unsigned int m_quadEBO;
        unsigned int m_modelVBO;

        //VAOs and VBOs for wireframe of boc and circle
        unsigned int m_wireframeSquareVAO;
        unsigned int m_wireframeCircleVAO;
        unsigned int m_wireframeSquareVBO;
        unsigned int m_wireframeCircleVBO;

        //Calculate model an view matrices
        glm::mat4 m_viewMatrix = glm::mat4(1.0f);
        glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

        //Shader program ID's for default rendering of a sprite
        //One for if the draw call is per sprite, one for if the draw call is for a batch to be drawn via instancing
        unsigned int m_defaultShaderID;
        unsigned int m_defaultInstanceShaderID;

        //Max number of spites that can be drawn total
        const int m_MAX_SPRITES = 100000;

        //Cache for the calculated modelMatrices
        std::vector<glm::mat4> m_modelMatricesCache;

        //Minimum number of sprites required for batch (instanced) rendering to be used instead of individual draw calls
        const unsigned int m_MIN_SPRITES_FOR_INSTANCING = 10;

        //Scaling factor for translation of Box2D coords into OpenGL appropriate
        const float m_scalingFactor = 100.0f; //100 pixels per metre

        float m_aspect = 0.0f; //Aspect ratio to be used to set the projection matrix appropriately
    };
}