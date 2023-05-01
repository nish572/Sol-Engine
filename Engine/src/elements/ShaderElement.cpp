#include "resource/ShaderElement.h"

#include "Core.h"

namespace CoreShaderElement
{
	//ShaderElement has initializer list for any managed resources that require initializing
	//All Elements MUST have at least m_core private member
	ShaderElement::ShaderElement(std::shared_ptr<Sol::Core> core) : m_core(core) //Extend initializer list if necessary
	{
	}
	ShaderElement::~ShaderElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool ShaderElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (!corePtr->getLogElement())
			{
				std::cerr << "Failed to initialize ShaderElement: LogElement is a nullptr" << std::endl;
				return false;
			}
		}

		if (corePtr)
		{
			corePtr->getLogElement()->logInfo("[Shader] Successfully Initialized");
		}
		return true;
	}

	//Create shader program
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
            if (corePtr)
            {
				corePtr->getLogElement()->logError(std::string("[Shader] Failed to Read Shader File: ") + e.what());
			}
        }
        if (corePtr)
        {
            corePtr->getLogElement()->logInfo("[Shader] Successfully Read Vertex and Fragment Shader Files");
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        //Compile shaders
        unsigned int vertex, fragment;
        //Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        //Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        //Shader program
        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertex);
        glAttachShader(shaderProgramID, fragment);
        glLinkProgram(shaderProgramID);
        checkCompileErrors(shaderProgramID, "PROGRAM");
        //Delete the shaders as both linked into program now and no longer necessary
        glDetachShader(shaderProgramID, vertex);
        glDetachShader(shaderProgramID, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (corePtr)
        {
			corePtr->getLogElement()->logInfo("[Shader] Successfully Created Shader Program");
		}
        return shaderProgramID;
    }

    void ShaderElement::checkCompileErrors(unsigned int shader, const std::string& type)
    {
        int success;
        char infoLog[1024];

        auto corePtr = m_core.lock();

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                if (corePtr)
                {
                    corePtr->getLogElement()->logError(std::string("[Shader] Cannot Compile Shader Of Type: ") + type);
                    corePtr->getLogElement()->logInfo(std::string("[Shader] Info Log: ") + infoLog);
                }
            }
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo(std::string("[Shader] Successfully Compiled Shader Of Type: ") + type);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                if (corePtr)
                {
                    corePtr->getLogElement()->logError("[Shader] Cannot Link Program");
                    corePtr->getLogElement()->logInfo(std::string("[Shader] Info Log: ") + infoLog);
                }
            }
            if (corePtr)
            {
                corePtr->getLogElement()->logInfo("[Shader] Successfully Linked Program");
            }
        }
    }
}