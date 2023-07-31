#include "systems/RenderSystem.h"
#include "ecs/EcsElement.h"
#include <iostream>

namespace EcsRenderSystem
{
    RenderSystem::RenderSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
        : m_ecsElement(ecsElement), m_sharedVAO(0), m_sharedVBO(0), m_sharedEBO(0)
    {
        initialize();
    }

    RenderSystem::~RenderSystem()
    {
    }

    void RenderSystem::initialize()
    {   
        //Initialize the VAO, VBO, and EBO to be used for instanced rendering
        glGenVertexArrays(1, &m_sharedVAO);
        glBindVertexArray(m_sharedVAO);

        //VBO
        float vertices[] = {
            //Positions    //Texture Coordinates
             0.5f,  0.5f,    1.0f, 1.0f,
             0.5f, -0.5f,    1.0f, 0.0f,
            -0.5f, -0.5f,    0.0f, 0.0f,
            -0.5f,  0.5f,    0.0f, 1.0f
        };
        glGenBuffers(1, &m_sharedVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_sharedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //EBO
        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
        };
        glGenBuffers(1, &m_sharedEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sharedEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //Vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        //Initialise the instance buffer for model matrices
        const int MAX_SPRITES = 1000;
        glGenBuffers(1, &m_modelVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_modelVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

        //Specify instance attribute data
        //Loop 4 times since the mat4 is split into 4 lots of vec4
        //So effectively each vec4 is a vertex attribute and 4 of those make up the mat4
        for (unsigned int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(i + 2);
            glVertexAttribPointer(i + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
            glVertexAttribDivisor(i + 2, 1);
        }

        //Unbind the VAO
        glBindVertexArray(0);
    }

    //Need some form of model matrix calculations then buffering

    void RenderSystem::update(double deltaTime) {
        //Get all entities with both TransformComponent and SpriteComponent
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto spriteComponents = m_ecsElement->getAllComponentsOfType<SpriteComponent>();

        //Iterate over all SpriteComponents
        for (const auto& spritePair : spriteComponents) {
            auto entityID = spritePair.first;
            auto spriteComponent = spritePair.second;

            //Find the corresponding TransformComponent
            auto transformPair = transformComponents.find(entityID);

            //If the transform does exist, then execute below
            if (transformPair != transformComponents.end()) {
                auto transformComponent = transformPair->second;
                
            }
            //Otherwise log an error message
        }
    }

    void RenderSystem::fixedUpdate(double fixedTimestep) {
        //Not necessary for the RenderSystem as it doesn't require fixed updates
    }


    //Need to rewrite this to not calculate model matrices as they're done beforehand
    //Need to rewrite this to make only one draw call per texture id
    void RenderSystem::renderSprite(std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<SpriteComponent> spriteComponent) {
        //Calculate the model matrix (transformation) using the position, rotation, and scale from the TransformComponent
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, transformComponent->position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transformComponent->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, transformComponent->scale);

        //Bind the shared VAO
        glBindVertexArray(m_sharedVAO);

        //Bind the texture for the sprite
        glBindTexture(GL_TEXTURE_2D, spriteComponent->textureID);

        //Use the shader program for the sprite
        glUseProgram(spriteComponent->shaderProgram);

        //Set the model matrix in the shader program
        GLuint modelLocation = glGetUniformLocation(spriteComponent->shaderProgram, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        //Draw the sprite
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Unbind the VAO
        glBindVertexArray(0);
    }
}