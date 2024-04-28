#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <memory>
#include <string>
#include <unordered_map>

//External library GLAD
#include <glad.h>

//The main resource currently being used is Texture Resource for sprite rendering

//Forward declaration of Core class
//This tells compiler Sol::Core exists without providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h here would introduce circular dependency
namespace Sol {
	class Core;
}

//Resource structs so resources can be cached
struct Resource {
	//Resource type enum for identifying resource type
	enum class ResourceType {
		Texture,
		Shader
	};

	ResourceType type;
	//Reference count for resource, default 0
	int refCount{ 0 };

	Resource(ResourceType resourceType) : type(resourceType) {}
};

//Texture resource that holds the texture ID, width and height, and the number of channels of the texture
//To be used for sprite rendering
struct TextureResource : public Resource {
	//textureID is the OpenGL texture ID
	GLuint textureID;
	//width, height, nrChannels are the width, height, and number of channels of the texture
	int width;
	int height;
	int nrChannels;
	TextureResource() : Resource(ResourceType::Texture), textureID(0), width(0), height(0), nrChannels(0) {}
};

//ShaderResource that holds the shader program ID
//Not currently used
struct ShaderResource : public Resource {
	int shaderProgramID;

	ShaderResource() : Resource(ResourceType::Shader), shaderProgramID(0) {}
};

namespace CoreResourceElement
{
	//Represents the Element responsible for managing the Resource operations in the Sol Engine
	class ResourceElement
	{
	public:
		ResourceElement(std::shared_ptr<Sol::Core> core);
		~ResourceElement();

		//Get resource from cache without loading it, useful if it is known the resource is already loaded
		std::shared_ptr<Resource> getResource(const std::string& filePath);

		//Load image resource by the file path
		std::shared_ptr<Resource> loadImage(const std::string& filePath);

		//Load shader resource by the file path
		std::shared_ptr<ShaderResource> loadShader(const std::string& vertexPath, const std::string& fragmentPath);

		//Load texture resource by the file path
		std::shared_ptr<TextureResource> loadTextureResource(const std::string& filePath);

		//Unload resource, decrement refCount and unload resource if refCount is 0
		void unloadResource(const std::string& filePath);

		//Clear resource cache, clear the entire resource cache and release all resources
		void clearCache();

		ENGINE_API bool initialize();

		//Call terminate to deallocate any of Resource Element's resources
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		//Resource cache where the string associated with the resource is the key and the shared pointer to the resource is the value
		std::unordered_map<std::string, std::shared_ptr<Resource>> m_resourceCache;
	};
} 