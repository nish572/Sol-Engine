#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>

#include <fstream>
#include <sstream>
#include <iostream>

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
	//More of a utility class than an Element, however it's functionality could be expanded in the future
	class ShaderElement
	{
	public:
		//Instantiate ShaderElement
		ShaderElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the ShaderElement instance
		~ShaderElement();

		//Initialize ShaderElement
		//Call this after calling ShaderElement's attachElement
		ENGINE_API bool initialize();

		//Returns program ID of Vert-Frag Shader program
		unsigned int createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

		void checkCompileErrors(unsigned int shader, const std::string& type);

		//Terminate ShaderElement
		//Call this to deallocate any of ShaderElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
	};
}