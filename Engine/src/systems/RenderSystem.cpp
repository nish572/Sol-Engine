#include "systems/RenderSystem.h"
#include "ecs/EcsElement.h"
#include <iostream>
#include <algorithm>

namespace EcsRenderSystem
{
    RenderSystem::RenderSystem(std::shared_ptr<CoreEcsElement::EcsElement> ecsElement)
        : m_ecsElement(ecsElement), m_mainVAO(0), m_quadVBO(0), m_quadEBO(0)
    {
        initialize();
    }

    RenderSystem::~RenderSystem()
    {
    }

    void RenderSystem::initialize()
    {
        //Creating the two shader program ID's to be used
        //One for if instances are being used, one for if not
        //Check though to see if the sprite's shaderProgram field is 0, if it isn't then render using that shader but do so in a single draw call not instanced draw call

        //Vertex and fragment shader source for single drawing
        const char* singleDrawVertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;

        uniform mat4 uModelMatrix;

        out vec2 TexCoords;

        void main()
        {
            gl_Position = uModelMatrix * vec4(aPos, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
        )glsl";
        //The fragment shader source can be the same as the single rendering fragment shader, but included for differentiation
        const char* singleDrawFragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoords;

        uniform sampler2D spriteTexture;

        void main()
        {    
            FragColor = texture(spriteTexture, TexCoords);
        }
        )glsl";

        //Vertex and fragment shader source for multi drawing
        const char* multiDrawVertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        layout (location = 2) in mat4 aModelMatrix;

        out vec2 TexCoords;

        void main()
        {
            gl_Position = aModelMatrix * vec4(aPos, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
        )glsl";
        //The fragment shader source can be the same as the single rendering fragment shader, but included for differentiation
        const char* multiDrawFragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoords;

        uniform sampler2D spriteTexture;

        void main()
        {    
            FragColor = texture(spriteTexture, TexCoords);
        }
        )glsl";


        m_defaultShaderID = createShader(singleDrawVertexShaderSource, singleDrawFragmentShaderSource);
        m_defaultInstanceShaderID = createShader(multiDrawVertexShaderSource, multiDrawFragmentShaderSource);

        //Initialize the VAO, VBO, and EBO to be used for instanced rendering
        //VAO for managing VBOs
        //VBO as data container for vertex or instance attributes
        //EBO for indices to process vertices
         
        //Generate VAO
        glGenVertexArrays(1, &m_mainVAO);
        glBindVertexArray(m_mainVAO);

        //Generate VBO
        //Originally wrote the positions as (x,y) since 2D, but for depth testing and in the future projection/view matrices (x,y,z) needed
        //Basic quad vertices for mapping textures to
        float vertices[] = {
            //Positions      //Texture Coordinates
            0.5f, 0.5f, 0.0f,  1.0f, 1.0f, // Vertex 1
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Vertex 3
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f // Vertex 4
        };
        //Gen, bind, and buffer so ready
        glGenBuffers(1, &m_quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Static as we are not changing the vertices' values

        //Generate EBO
        //Indices for procesing vertex order
        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
        };
        //Gen, bind, and buffer so ready
        glGenBuffers(1, &m_quadEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); //Static as we are not changing the indices' values

        //Specify vertex attribute data
        //Layout location 0 is the 3 positions (x,y,z)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        //Layout location 1 is the 2 texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        //Initialise the instance buffer for model matrices
       
        glGenBuffers(1, &m_modelVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_modelVBO);
        //Required for OpenGL to allocate the maximum potential memory possibly needed
        glBufferData(GL_ARRAY_BUFFER, m_MAX_SPRITES * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW); //Dynamic as the matrices data will be change a lot

        //Specify instance attribute data
        //Loop 4 times since the mat4 is split into 4 lots of vec4
        //So effectively each vec4 is a vertex attribute and 4 of those make up the mat4
        //In this case since the vertices' positional data is in layout location 0, and and the texture coords are in layout location 1, need to start at 2
        //And increment this by 1 each time since there are 4 vec4 to be stored
        //First vec4 will be in layout 2, then the next in layout 3, and so on
        //Effectively:
        //Layout 2 is like an array of the first vec4 of every matrix being processed, layout 3 gets the second, and so forth
        //And then 
        for (unsigned int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(i + 2);
            //Set which attribute to modify, then number of values per attribute, 
            //then data type of those values, normalisation status, stride (offset in bytes between consecutive attributes),
            //then the pointer which specifies the offset of the first value of the first generic vertex attribute in the array in the data store of the buffer
            glVertexAttribPointer(i + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
            //Attributes per instance not per vertex
            glVertexAttribDivisor(i + 2, 1);
        }

        //Unbind the VAO
        glBindVertexArray(0);
    }

    void RenderSystem::update(double deltaTime) {
        //Get all entities with both TransformComponent and SpriteComponent
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto spriteComponents = m_ecsElement->getAllComponentsOfType<SpriteComponent>();

        std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs;

        //Iterate over all SpriteComponents
        for (const auto& spritePair : spriteComponents) {
            //Find the corresponding TransformComponent
            auto transformPair = transformComponents.find(spritePair.first);

            //If the transform does exist, then execute below
            if (transformPair != transformComponents.end()) {
                tmpSpriteTransformPairs.push_back(std::make_pair(spritePair.second, transformPair->second));
            }
            //Otherwise log an error message
        }

        //Sort the temporary vector of sprite-transform pairs
        std::sort(tmpSpriteTransformPairs.begin(), tmpSpriteTransformPairs.end(), //Provides the range of the sort
            [](const auto& a, const auto& b) { //Input parameters to the lambda function
                return a.first->textureID < b.first->textureID; //Condition for sorting, i.e. sorting by textureID of sprite
            });

        //Clear m_modelMatricesCache as otherwise it will infinitely increase
        m_modelMatricesCache.clear();

        //Calculate model matrices
        for (size_t i = 0; i < tmpSpriteTransformPairs.size(); i++) {
            auto& tmpPair = tmpSpriteTransformPairs[i];

            //Calculate the model matrix (transformation) using the position, rotation, and scale from the TransformComponent
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, tmpSpriteTransformPairs[i].second->position);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(tmpSpriteTransformPairs[i].second->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            modelMatrix = glm::scale(modelMatrix, tmpSpriteTransformPairs[i].second->scale);

            m_modelMatricesCache.push_back(modelMatrix);
        }

        //Test code if 
        /*for (const auto& mod : modelMatrices) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout << mod[i][j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl << std::endl;
        }*/

        renderSprites(tmpSpriteTransformPairs);
    }

    void RenderSystem::fixedUpdate(double fixedTimestep) {
        //Not necessary for the RenderSystem as it doesn't require fixed updates
        //Could have done a base class and inherited from this however that would have required a big chunk of refactoring that is unnecessary right now
    }

    unsigned int RenderSystem::createShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
        //Vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        //Check for vertex shader compilation errors
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "[ECS - Render System] Cannot Compile Vertex Shader" << std::endl << "Info Log : " << infoLog << std::endl;
        }
        //Fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        //Check for fragment shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "[ECS - Render System] Cannot Compile Fragment Shader" << std::endl << "Info Log : " << infoLog << std::endl;
        }
        //Link fragment and vertex shaders
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        //Check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "[ECS - Render System] Cannot Link Program" << std::endl << "Info Log : " << infoLog << std::endl;
        }
        //Delete shaders since program has been created
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return shaderProgram;
    }

    void RenderSystem::renderSprites(std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs) {
        GLsizei numSpritesByTexture = 1;
        int currentTexture = tmpSpriteTransformPairs[0].first->textureID;
        size_t numSpritesRenderedBeforeThisBatch = 0;

        //Bind the shared VAO
        glBindVertexArray(m_mainVAO);

        for (size_t i = 0; i < tmpSpriteTransformPairs.size(); i++) {

            //If the next sprite has a different textureID, render the batch using the current texture ID
            //If the number of sprites in the batch is too little for instanced rendering by the textureID to be worth it, then draw individually
            //If the number of sprites in the batch is high enough for instanced rendering by the textureID to be worth it, buffer the batch, then draw instanced
            if (tmpSpriteTransformPairs[i].first->textureID != currentTexture) {
                
                //If there aren't enough sprites in this batch to make the benefit of instanced rendering greater than the deficit of the overhead of buffering the model matrices then
                //For every sprite in the right range (essentially offset by the number of sprites already rendered in this frame before this batch (not including any in this batch)
                //Render those sprites one by one
                if (numSpritesByTexture < m_MIN_SPRITES_FOR_INSTANCING) {
                    singleDraw(currentTexture, numSpritesByTexture, numSpritesRenderedBeforeThisBatch);
                }

                //If there are enough sprites in this batch to make the benefit of instanced rendering greater than the deficit of the overhead of buffering the model matrices then
                else {
                    multiDraw(currentTexture, numSpritesByTexture, numSpritesRenderedBeforeThisBatch);
                }

                //When the batch is finished drawing, update the numSpritesRenderedBeforeThisBatch
                numSpritesRenderedBeforeThisBatch += numSpritesByTexture;
                //Set current texture to the ID of the next texture, and reset numSpritesByTexture
                currentTexture = tmpSpriteTransformPairs[i].first->textureID;
                numSpritesByTexture = 1;
            }
            //Checking the one after, but also making sure the one after won't be out of bounds
            if (i != tmpSpriteTransformPairs.size() - 1 && tmpSpriteTransformPairs[i + 1].first->textureID == currentTexture) {
                numSpritesByTexture++;
            }
        }

        //Need to render the final batch as the loop won't render the final batch due to there being no next sprite texture ID to check
        //If the number of sprites in the batch is too little for instanced rendering by the textureID to be worth it, then draw individually
        //If the number of sprites in the batch is high enough for instanced rendering by the textureID to be worth it, buffer the batch, then draw instanced
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        if (numSpritesByTexture < m_MIN_SPRITES_FOR_INSTANCING) {
            singleDraw(currentTexture, numSpritesByTexture, numSpritesRenderedBeforeThisBatch);
        }
        //If there are enough sprites in this batch to make the benefit of instanced rendering greater than the deficit of the overhead of buffering the model matrices then
        else {
            multiDraw(currentTexture, numSpritesByTexture, numSpritesRenderedBeforeThisBatch);
        }

        //Unind the shared VAO
        glBindVertexArray(0);
    }

    void RenderSystem::singleDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch) {
        //Bind the texture for this batch of sprites
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glUseProgram(m_defaultShaderID);
        //Get uniform location
        GLint modelLoc = glGetUniformLocation(m_defaultShaderID, "uModelMatrix");
        //Looping through the correct section of the model matrices cache, and rendering one by one
        for (size_t j = numSpritesRenderedBeforeThisBatch; j < numSpritesRenderedBeforeThisBatch + numSpritesByTexture; j++) {
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatricesCache[j]));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        //Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void RenderSystem::multiDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch) {
        //Buffer the model matrix data of these sprites to the GPU to be used for instanced rendering
        //Add all model matrices in the appropriate range to a temporary store, to be used to buffer sub data
        std::vector<glm::mat4> tmpModelMatricesVec(m_modelMatricesCache.begin() + numSpritesRenderedBeforeThisBatch,
            m_modelMatricesCache.begin() + numSpritesRenderedBeforeThisBatch + numSpritesByTexture);
        glBindBuffer(GL_ARRAY_BUFFER, m_modelVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, tmpModelMatricesVec.size() * sizeof(glm::mat4), tmpModelMatricesVec.data());
        tmpModelMatricesVec.clear();
        //Bind the texture for this batch of sprites
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glUseProgram(m_defaultInstanceShaderID);
        //Draw the batch of sprites via instanced rendering
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, numSpritesByTexture);
        //Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}