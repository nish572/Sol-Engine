//------- Render System -----
//Implements Rendering
//For The Sol Core Engine
//---------------------------

#include "systems/RenderSystem.h"
#include "ecs/EcsElement.h"
#include <iostream>
#include <algorithm>

#include "Core.h"

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
        //Creating the two shader program IDs to be used
        //One for if batch rendering is to being used, one for if not
        //Check though to see if the sprite's shaderProgram field is 0, if it isn't then render using that shader but do so in a single draw call not instanced draw call
        
        setProjectionMatrix(); //Initial setting of the default project matrix, however this function is called every update to ensure it doesn't need to be changed

        //Vertex shader source for single drawing
        const char* singleDrawVertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;

        uniform mat4 uModelMatrix;
        
        uniform mat4 view;
        uniform mat4 projection;
        uniform bool useWireframe; //Uniform to toggle wireframe rendering

        out vec2 TexCoords;

        void main()
        {
            gl_Position = projection * view * uModelMatrix * vec4(aPos, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
        )glsl";
        //Fragment shader source for single drawing
        const char* singleDrawFragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoords;

        uniform sampler2D spriteTexture;
        uniform vec4 lineColor;
        uniform bool useWireframe;

        void main()
        {
            if (useWireframe)
            {
                FragColor = lineColor;
            } else {
                FragColor = texture(spriteTexture, TexCoords);
            }
        }
        )glsl";

        //Vertex shader source for multi drawing
        const char* multiDrawVertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        layout (location = 2) in mat4 aModelMatrix;

        uniform mat4 view;
        uniform mat4 projection;

        out vec2 TexCoords;

        void main()
        {
            gl_Position = projection * view * aModelMatrix * vec4(aPos, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
        )glsl";
        //Fragment shader source for multi drawing
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

        //Create the two shaders, one for single drawing and one for multi drawing
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Static as we not changing the vertices' values

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
        glBufferData(GL_ARRAY_BUFFER, m_MAX_SPRITES * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW); //Dynamic as the matrices data will change a lot

        //Specify instance attribute data
        //Loop 4 times since the mat4 is split into 4 lots of vec4
        //So effectively each vec4 is a vertex attribute and 4 of those make up the mat4
        //In this case since the vertices' positional data is in layout location 0, and the texture coords are in layout location 1, need to start at 2
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

        //Define vertices for a wireframe square
        float squareVertices[] = {
            0.5f, 0.5f, 0.0f,  // Top right
            0.5f, -0.5f, 0.0f, // Bottom right
            -0.5f, -0.5f, 0.0f, // Bottom left
            -0.5f, 0.5f, 0.0f  // Top left
        };

        //Generate VAO and VBO for the wireframe square
        glGenVertexArrays(1, &m_wireframeSquareVAO);
        glBindVertexArray(m_wireframeSquareVAO);

        glGenBuffers(1, &m_wireframeSquareVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_wireframeSquareVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

        //Specify vertex attribute data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        //Unbind the VAO
        glBindVertexArray(0);

        //Circle parameters, number of segments to divide the circle into, more segments means a smoother circle
        const int numSegments = 32;
        //Define vertices for a wireframe circle
        float circleVertices[3 * numSegments]; //Vertex count for the circle

        for (int i = 0; i < numSegments; ++i) {
            float angle = 2.0f * M_PI * float(i) / float(numSegments);
            circleVertices[3 * i] = 0.5f * cosf(angle); //0.5f is the radius
            circleVertices[3 * i + 1] = 0.5f * sinf(angle);
            circleVertices[3 * i + 2] = 0.0f;
        }

        //Generate VAO and VBO for the wireframe circle
        glGenVertexArrays(1, &m_wireframeCircleVAO);
        glBindVertexArray(m_wireframeCircleVAO);

        glGenBuffers(1, &m_wireframeCircleVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_wireframeCircleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

        //Specify vertex attribute data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        //Unbind the VAO
        glBindVertexArray(0);
    }

    //Check if the aspect ratio has changed and if it has, update the projection matrix accordingly
    void RenderSystem::setProjectionMatrix()
    {
        float aspect = ApplicationConfig::Config::screenWidth / ApplicationConfig::Config::screenHeight;
        if (aspect != m_aspect)
        {
            float halfHeight = ApplicationConfig::Config::screenHeight / 2.0f;
            float halfWidth = halfHeight * aspect;

            m_projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f); //Set the projection matrix to orthographic and base upon the screen dimensions
            m_aspect = aspect;
        }
    }

    void RenderSystem::update(double deltaTime) {
        //Check is projection matrix should be changed and if so, change
        setProjectionMatrix();
        //Get all Entities with both TransformComponent and SpriteComponent
        auto transformComponents = m_ecsElement->getAllComponentsOfType<TransformComponent>();
        auto spriteComponents = m_ecsElement->getAllComponentsOfType<SpriteComponent>();
        auto colliderComponents = m_ecsElement->getAllComponentsOfType<ColliderComponent>(); //For use in rendering wireframes if made visible

        std::vector<std::pair<std::shared_ptr<SpriteComponent>, std::shared_ptr<TransformComponent>>> tmpSpriteTransformPairs;

        //Sprite rendering
        for (const auto& spritePair : spriteComponents) {
            auto transformPair = transformComponents.find(spritePair.first);
            if (transformPair != transformComponents.end()) {
                tmpSpriteTransformPairs.push_back(std::make_pair(spritePair.second, transformPair->second));
            }
        }

        //New loop for rendering colliders irrespective of sprite components
        for (const auto& colliderPair : colliderComponents) {
            auto transformPair = transformComponents.find(colliderPair.first);
            if (transformPair != transformComponents.end() && colliderPair.second->isVisible) {
                bool isCircle = colliderPair.second->shapeType == ShapeType::Circle;
                renderWireframe(*(transformPair->second), *(colliderPair.second), isCircle);
            }
        }

        //Sort the temporary vector of sprite-transform pairs
        std::sort(tmpSpriteTransformPairs.begin(), tmpSpriteTransformPairs.end(), //Provides the range of the sort
            [](const auto& a, const auto& b) { //Input parameters to the lambda function
                return a.first->textureID < b.first->textureID; //Condition for sorting, i.e. sorting by textureID of sprite
            });

        //Clear m_modelMatricesCache as otherwise it will infinitely increase
        m_modelMatricesCache.clear();

        //Calculate model matrices
        //Used to manipulate sprites based on transformational data from the transform component
        for (size_t i = 0; i < tmpSpriteTransformPairs.size(); i++) {
            auto& tmpPair = tmpSpriteTransformPairs[i];
            //Calculate the model matrix (transformation) using the position, rotation, and scale from the TransformComponent
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            
            modelMatrix = glm::translate(modelMatrix, tmpPair.second->position);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(tmpPair.second->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
               
            modelMatrix = glm::scale(modelMatrix, tmpPair.second->scale);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(tmpPair.first->size.x, tmpPair.first->size.y, 0));

            m_modelMatricesCache.push_back(modelMatrix);
        }

        //Render sprites, but check first to ensure there are any appropriate sprites to be rendered
        if (tmpSpriteTransformPairs.size() > 0)
        {
            renderSprites(tmpSpriteTransformPairs);
        }
    }

    void RenderSystem::fixedUpdate(double fixedTimestep) {
        //Not necessary for the Render System as it doesn't require fixed updates
    }

    //For use in creating the default shaders for single drawing and multi drawing
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

    //Determine whether to use single drawing or multi drawing depending on the number of sprites to be rendered (with the same texture)
    //Make calls to these functions based on this decision
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

    //Non-instanced rendering
    void RenderSystem::singleDraw(int currentTexture, GLsizei numSpritesByTexture, size_t numSpritesRenderedBeforeThisBatch) {
        //Bind the texture for this batch of sprites
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glUseProgram(m_defaultShaderID);
        //Get uniform location
        GLint modelLoc = glGetUniformLocation(m_defaultShaderID, "uModelMatrix");
        GLint viewLoc = glGetUniformLocation(m_defaultShaderID, "view");
        GLint projLoc = glGetUniformLocation(m_defaultShaderID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
        //Looping through the correct section of the model matrices cache, and rendering one by one
        for (size_t j = numSpritesRenderedBeforeThisBatch; j < numSpritesRenderedBeforeThisBatch + numSpritesByTexture; j++) {

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatricesCache[j]));
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        //Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //Instanced (batch) rendering
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
        //Get uniform location
        GLint viewLoc = glGetUniformLocation(m_defaultInstanceShaderID, "view");
        GLint projLoc = glGetUniformLocation(m_defaultInstanceShaderID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
        //Draw the batch of sprites via instanced rendering
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, numSpritesByTexture);
        //Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //Render a square or cicle wireframe for an Entity whose collider is set to be visible in the Editor
    void RenderSystem::renderWireframe(const TransformComponent& transform, const ColliderComponent& collider, bool isCircle) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(transform.position.x, transform.position.y, 0.0f));
        model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 sizeInPixels;
        if (isCircle) {
            //Convert the radius from meters to pixels and calculate the diameter for scaling
            float diameterInPixels = collider.radius * 2.0f * m_scalingFactor;
            sizeInPixels = glm::vec3(diameterInPixels, diameterInPixels, 1.0f);
        }
        else {
            //Box collider sizing
            sizeInPixels = glm::vec3(collider.width * m_scalingFactor, collider.height * m_scalingFactor, 1.0f);
        }

        model = glm::scale(model, sizeInPixels); //Apply the size in pixels
        model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f)); //Then apply any additional scaling from the transform

        glUseProgram(m_defaultShaderID);

        GLint modelLoc = glGetUniformLocation(m_defaultShaderID, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(m_defaultShaderID, "uViewMatrix");
        GLint projLoc = glGetUniformLocation(m_defaultShaderID, "uProjectionMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));

        GLint useWireframeLoc = glGetUniformLocation(m_defaultShaderID, "useWireframe");
        GLint lineColorLoc = glGetUniformLocation(m_defaultShaderID, "lineColor");

        glUniform1i(useWireframeLoc, GL_TRUE);
        glUniform4f(lineColorLoc, 1.0f, 0.0f, 0.0f, 1.0f); //Red color for wireframe

        if (isCircle) {
            glBindVertexArray(m_wireframeCircleVAO);
            glDrawArrays(GL_LINE_LOOP, 0, 32); //Circle is made of 32 segments, as decided in the initialise function
        }
        else {
            glBindVertexArray(m_wireframeSquareVAO);
            glDrawArrays(GL_LINE_LOOP, 0, 4); //4 vertices for the square
        }
        //Unbind
        glUniform1i(useWireframeLoc, GL_FALSE);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}