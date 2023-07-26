#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>

#include <unordered_map>

#include <glad.h>


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
		Audio,
		Shader
	};

	ResourceType type;
	//Reference count for resource, default 0
	int refCount{ 0 };

	Resource(ResourceType resourceType) : type(resourceType) {}
};

struct TextureResource : public Resource {
	//textureID is the OpenGL texture ID
	GLuint textureID;
	//width, height, nrChannels are the width, height, and number of channels of the texture
	int width;
	int height;
	int nrChannels;
	TextureResource() : Resource(ResourceType::Texture), textureID(0), width(0), height(0), nrChannels(0) {}
};

//Fill in struct AudioResource here
//... 

//Fill in struct ShaderResource here
//Shader resource struct
struct ShaderResource : public Resource {
	int shaderProgramID;

	ShaderResource() : Resource(ResourceType::Shader), shaderProgramID(0) {}
};

namespace CoreResourceElement
{
	//Represents the element responsible for managing the Resource operations in the Sol Engine
	class ResourceElement
	{
	public:
		//Instantiate ResourceElement
		ResourceElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the ResourceElement instance
		~ResourceElement();

		//Get resource from cache without loading it, useful if we know the resource is already loaded
		std::shared_ptr<Resource> getResource(const std::string& filePath);

		//Load image resource by the file path
		std::shared_ptr<Resource> loadImage(const std::string& filePath);

		//Load audio resource by the file path
		//...

		//Load shader resource by the file path
		std::shared_ptr<Resource> loadShader(const std::string& vertexPath, const std::string& fragmentPath);

		//Load texture resource by the file path
		std::shared_ptr<TextureResource> loadTextureResource(const std::string& filePath);

		//Unload resource, decrement refCount and unload resource if refCount is 0
		void unloadResource(const std::string& filePath);

		//Clear resource cache, clear the entire resource cache and release all resources
		void clearCache();

		//Initialize ResourceElement
		//Call this after calling ResourceElement's attachElement
		ENGINE_API bool initialize();

		//Terminate ResourceElement
		//Call this to deallocate any of ResourceElement's resources
		//Call this when amending Core's detachElement function and inside Core's terminate function
		void terminate();

	private:
		//Pointer to Core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		//Resource cache
		std::unordered_map<std::string, std::shared_ptr<Resource>> m_resourceCache;
	};
} 