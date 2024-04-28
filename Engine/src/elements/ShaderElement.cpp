//------- Shader Element ----
//Shader System
//For The Sol Core Engine
//---------------------------

//Currently not in use

#include "resource/ShaderElement.h"

#include "Core.h"

namespace CoreShaderElement
{
	ShaderElement::ShaderElement(std::shared_ptr<Sol::Core> core) : m_core(core)
	{
	}
	ShaderElement::~ShaderElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, none
    bool ShaderElement::initialize()
    {
        auto corePtr = m_core.lock();
        if (corePtr)
        {
            if (corePtr->getLogElement())
            {
                m_logElementAttached = true;
            }
        }

        if (m_logElementAttached)
        {
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Initialized");
            }
            return true;
        }
        std::cout << "[Shader] Successfully Initialized" << std::endl;
		return true;
	}

	//Create shader program via a vertex shader file and a fragment shader file
    unsigned int ShaderElement::createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
    {
        unsigned int shaderProgramID;

        //Retrieve the vertex/fragment source code from paths
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        //Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        auto corePtr = m_core.lock();
        try
        {
            //Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            //Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            //Close file handlers
            vShaderFile.close();
            fShaderFile.close();
            //Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            //Log error if shader file(s) not successfully read
            if (m_logElementAttached)
            {
                if (corePtr)
                {
					corePtr->getLogElement()->logError(std::string("[Shader] Failed to Read Shader File(s): ") + e.what());
				}
            }
            else
            {
                std::cerr << "[Shader] Failed to Read Shader File(s): " << e.what() << std::endl;
            }
            throw std::runtime_error("[Shader] Failed to Read Shader File(s)");
        }
        //Log success if shader files successfully read
        if (m_logElementAttached)
        {
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Read Vertex and Fragment Shader Files");
            }
        }
        else
        {
			std::cout << "[Shader] Successfully Read Vertex and Fragment Shader Files" << std::endl;
		}

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        //Compile shaders
        unsigned int vertex, fragment;
        //Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkShaderCompilationErrors(vertex, "VERTEX");
        //Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkShaderCompilationErrors(fragment, "FRAGMENT");
        //Shader program
        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertex);
        glAttachShader(shaderProgramID, fragment);
        glLinkProgram(shaderProgramID);
        checkProgramLinkErrors(shaderProgramID);
        //Delete the shaders as both linked into program now and no longer necessary
        glDetachShader(shaderProgramID, vertex);
        glDetachShader(shaderProgramID, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (m_logElementAttached)
        {
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Created Shader Program");
            }
            return shaderProgramID;
        }
        std::cout << "[Shader] Successfully Created Shader Program" << std::endl;
        return shaderProgramID;        
    }

    //Auxilliary function to check shader compilation errors
    void ShaderElement::checkShaderCompilationErrors(unsigned int shader, const std::string& type)
    {
        int success;
        char infoLog[1024];

        auto corePtr = m_core.lock();

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            if (m_logElementAttached) {
                if (corePtr)
                {
                    corePtr->getLogElement()->logError("[Shader] Cannot Compile Shader Of Type: " + type);
                    corePtr->getLogElement()->logInfo(std::string("[Shader] Info Log: ") + infoLog);
                }
            }
            else {
                std::cerr << "[Shader] Cannot Compile Shader Of Type: " << type << std::endl;
                std::cout << "[Shader] Info Log : " << infoLog << std::endl;
            }
            throw std::runtime_error("[Shader] Cannot Compile Shader Of Type: " + type);
        }
        if (m_logElementAttached) {
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Compiled Shader Of Type: " + type);
            }
        }
        else {
            std::cout << "[Shader] Successfully Compiled Shader Of Type: " << std::endl;
        }
    }

    //Auxilliary function to check shader program link errors
    void ShaderElement::checkProgramLinkErrors(unsigned int program)
    {
        int success;
        char infoLog[1024];

        auto corePtr = m_core.lock();

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            if (m_logElementAttached) {
                if (corePtr)
                {
                    corePtr->getLogElement()->logError("[Shader] Cannot Link Program");
                    corePtr->getLogElement()->logInfo(std::string("[Shader] Info Log: ") + infoLog);
                }
            }
            else {
                std::cerr << "[Shader] Cannot Link Program" << std::endl;
                std::cout << "[Shader] Info Log : " << infoLog << std::endl;
            }
            throw std::runtime_error("[Shader] Cannot Link Program");
        }
        if (m_logElementAttached) {
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Linked Program");
            }
        }
        else {
            std::cout << "[Shader] Successfully Linked Program" << std::endl;
        }
    }
}