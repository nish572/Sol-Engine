#include "systems/RenderSystem.h"
#include "ecs/EcsElement.h"
#include <iostream>
#include <algorithm>

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

        std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs;

        //Iterate over all SpriteComponents
        for (const auto& spritePair : spriteComponents) {
            auto entityID = spritePair.first;
            auto spriteComponent = spritePair.second;

            //Find the corresponding TransformComponent
            auto transformPair = transformComponents.find(entityID);

            //If the transform does exist, then execute below
            if (transformPair != transformComponents.end()) {
                auto transformComponent = transformPair->second;
                tmpSpriteTransformPairs.push_back(std::make_pair(spritePair.second, transformPair->second));
            }
            //Otherwise log an error message
        }

        //Sort the temporary vector of sprite-transform pairs
        std::sort(tmpSpriteTransformPairs.begin(), tmpSpriteTransformPairs.end(), //Provides the range of the sort
            [](const auto& a, const auto& b) { //Input parameters to the lambda function
                return a.first->textureID < b.first->textureID; //Condition for sorting, i.e. sorting by textureID of sprite
            });

        //Calculate model matrices
        std::vector<glm::mat4> modelMatrices;
        for (size_t i = 0; i < tmpSpriteTransformPairs.size(); i++) {
            auto& tmpPair = tmpSpriteTransformPairs[i];

            //Calculate the model matrix (transformation) using the position, rotation, and scale from the TransformComponent
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, tmpSpriteTransformPairs[i].second->position);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(tmpSpriteTransformPairs[i].second->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            modelMatrix = glm::scale(modelMatrix, tmpSpriteTransformPairs[i].second->scale);

            modelMatrices.push_back(modelMatrix);
        }

        //Buffer this model matrix data to the GPU to be used for rendering
        glBindBuffer(GL_ARRAY_BUFFER, m_modelVBO);
        glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        renderSprites(tmpSpriteTransformPairs);
    }

    void RenderSystem::fixedUpdate(double fixedTimestep) {
        //Not necessary for the RenderSystem as it doesn't require fixed updates
        //Could have done a base class and inherited from this however that would have required a big chunk of refactoring that is unnecessary right now
    }

    void RenderSystem::renderSprites(std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs) {
        unsigned int numSpritesByTexture = 1;
        unsigned int currentTexture = tmpSpriteTransformPairs[0].first->textureID;
        GLuint currentShaderProgram = tmpSpriteTransformPairs[0].first->shaderProgram;  // <-- Store the current shader program ID

        for (size_t i = 0; i < tmpSpriteTransformPairs.size(); i++) {
            //std::cout << currentTexture << std::endl;
            if (tmpSpriteTransformPairs[i].first->textureID != currentTexture) {
                //Bind the shared VAO
                glBindVertexArray(m_sharedVAO);

                //Bind the texture for this batch of sprites
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, currentTexture);

                //Use the shader program for the sprite
                glUseProgram(currentShaderProgram);

                //Draw the batch of sprites
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, numSpritesByTexture);

                currentTexture = tmpSpriteTransformPairs[i].first->textureID;
                currentShaderProgram = tmpSpriteTransformPairs[i].first->shaderProgram;  // <-- Store the new shader program ID
                numSpritesByTexture = 1;
            }
            //Checking the one after, but also making sure the one after won't be out of bounds
            if (i != tmpSpriteTransformPairs.size() - 1 && tmpSpriteTransformPairs[i + 1].first->textureID == currentTexture) {
                numSpritesByTexture++;
            }
        }

        //Bind the shared VAO
        glBindVertexArray(m_sharedVAO);

        //Bind the texture for this batch of sprites
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        //Use the shader program for the sprite
        glUseProgram(currentShaderProgram);

        //Draw the batch of sprites
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, numSpritesByTexture);

        //Unbind the VAO
        glBindVertexArray(0);
    }
}