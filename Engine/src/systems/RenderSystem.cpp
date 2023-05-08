#include "systems/RenderSystem.h"
#include "ecs/EcsElement.h"
#include "ecs/Components.h"

namespace EcsRenderSystem
{
    RenderSystem::RenderSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
        : m_ecsElement(ecsElement), vertices({
        // Positions          // Texture Coords
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f   // Top-left
            }), indices({
                0, 1, 2, // First triangle
                0, 2, 3  // Second triangle
                })
    {

        // Set up the VAO for rendering sprites
        glGenVertexArrays(1, &m_spriteVAO);
        glBindVertexArray(m_spriteVAO);

        // Set up the VBO for base vertices
        glGenBuffers(1, &m_spriteVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Set up the attribute pointers for base vertices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Set up the EBO for base vertices
        glGenBuffers(1, &m_spriteEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_spriteEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    RenderSystem::~RenderSystem() {
        // Clean up OpenGL resources
        glDeleteBuffers(1, &m_spriteEBO);
        glDeleteBuffers(1, &m_spriteVBO);
        glDeleteVertexArrays(1, &m_spriteVAO);
    }

    void RenderSystem::update(double deltaTime) {
        // Get all entities with SpriteComponents
        auto spriteComponents = m_ecsElement->getAllComponentsOfType<SpriteComponent>();

        // Iterate through all SpriteComponents and render them
        for (const auto& pair : spriteComponents) {
            Entity entity = pair.first;
            std::shared_ptr<SpriteComponent> spriteComponent = pair.second;

            // Get the TransformComponent of the entity
            TransformComponent& transform = m_ecsElement->getComponent<TransformComponent>(entity);

            // Update the instance VBO with the new transformation matrix and texture ID
            glBindBuffer(GL_ARRAY_BUFFER, spriteComponent->instanceVBO);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), transform.position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::scale(glm::mat4(1.0f), transform.scale);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(modelMatrix));
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::mat4), sizeof(GLuint), &spriteComponent->textureID);

            // Render the sprite
            renderSprite(spriteComponent);
        }
    }

    void RenderSystem::addSpriteComponent(Entity entity, std::shared_ptr<CoreEcsElement::SpriteComponent> spriteComponent, GLuint textureID) {
        // Set up the instance VBO
        glGenBuffers(1, &spriteComponent->instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, spriteComponent->instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) + sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

        // Set up the attribute pointers for the instance data
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glEnableVertexAttribArray(2 + i);
            glVertexAttribDivisor(2 + i, 1);
        }
        glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)sizeof(glm::mat4));
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(6, 1);

        // Set the texture ID for the SpriteComponent
        spriteComponent->textureID = textureID;

        // Add the SpriteComponent to the entity
        m_ecsElement->addComponent<SpriteComponent>(entity, *spriteComponent);
    }

    void RenderSystem::renderSprite(std::shared_ptr<CoreEcsElement::SpriteComponent> spriteComponent) {
        glBindVertexArray(m_spriteVAO);

        // Use the shader program associated with the SpriteComponent
        glUseProgram(spriteComponent->shaderProgram);

        // Bind the texture associated with the SpriteComponent
        glBindTexture(GL_TEXTURE_2D, spriteComponent->textureID);

        // Draw the sprite using instanced rendering
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);

        // Unbind the texture and shader program
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        glBindVertexArray(0);
    }
}