#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//External library GLAD
#include <glad.h>

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreShaderElement
{
	//Represents the element responsible for managing the Shader creation operation in the Sol Engine
	//More of a utility class than an Element, however its functionality may be expanded in the future
	class ShaderElement
	{
	public:
		ShaderElement(std::shared_ptr<Sol::Core> core);
		~ShaderElement();

		ENGINE_API bool initialize();

		//Returns program ID of Vert-Frag Shader program
		unsigned int createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

		//Auxilliary functions to check any shader compilation or linking errors
		void checkShaderCompilationErrors(unsigned int shader, const std::string& type);
		void checkProgramLinkErrors(unsigned int program);

		//No terminate function since Shader Element is more of a utility class and has no particular resources of its own

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
	};
}