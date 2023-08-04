#include "resource/ResourceElement.h"

#include "Core.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"


namespace CoreResourceElement
{
	//ResourceElement has initializer list for any managed resources that require initializing
	//All Elements MUST have at least m_core private member
	ResourceElement::ResourceElement(std::shared_ptr<Sol::Core> core) : m_core(core)//Extend initializer list if necessary
	{
	}
	ResourceElement::~ResourceElement()
	{
	}

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, e.g. RenderElement's initialize function requires window height and width
	bool ResourceElement::initialize()
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
			corePtr->getLogElement()->logInfo("[Resource] Successfully Initialized");
			return true;
		}
		std::cout << "[Resource] Successfully Initialized" << std::endl;
		return true;
	}

	//Get a resource from the cache, useful if we know the resource is already loaded
	std::shared_ptr<Resource> ResourceElement::getResource(const std::string& filePath)
	{
		//Check if the resource is already in the cache
		auto it = m_resourceCache.find(filePath);
		//If the resource is in the cache, return it
		if (it != m_resourceCache.end())
		{
			//Return the resource
			return it->second;
		}
		//If the resource is not in the cache, return nullptr
		if (m_logElementAttached)
		{
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Resource] Failed to Get Resource: " + filePath);
			}
			return nullptr;
		}
		std::cerr << "[Resource] Failed to Get Resource: " << filePath << std::endl;
		return nullptr;
	}

	//Load an image from a file path, returns a TextureResource object
	std::shared_ptr<Resource> ResourceElement::loadImage(const std::string& filePath)
	{
		//Check if the image is already in the cache
		auto it = m_resourceCache.find(filePath);
		//If the image is in the cache, increment the refCount and return the resource
		if (it != m_resourceCache.end())
		{
			//Image already in cache, increment refCount and return the resource
			it->second->refCount++;
			return it->second;
		}

		//Load the image using stb_image
		int width, height, nrChannels;
		//stbi_load returns a pointer to the image data
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

		//Create the texture with OpenGL and get the texture ID
		GLuint textureID;
		//Generate the texture
		glGenTextures(1, &textureID);
		//Bind the texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		//Texture filtering and wrapping
		//linear filtering is smooth but provide option for gl_nearest if making pixel game
		//for now clamp to edge is fine which may stretch edges hypothetically but for now that's not an issue
		//currently not making use of mipmaps, to implement shortly
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Set the internal format and format parameters based on the number of channels
		GLenum internalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		GLenum format = internalFormat;

		//Set the texture parameters and generate mipmaps
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Free the image data
		stbi_image_free(data);

		//Create a TextureResource object with the texture ID and other necessary information
		auto textureResource = std::make_shared<TextureResource>();
		textureResource->textureID = textureID;
		textureResource->width = width;
		textureResource->height = height;
		textureResource->nrChannels = nrChannels;

		textureResource->refCount = 1;

		//Insert the new resource into the cache
		m_resourceCache[filePath] = textureResource;

		//If the resource is created and stored in cache properly, return the resource
		if (textureResource)
		{
			if (m_logElementAttached)
			{
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					corePtr->getLogElement()->logInfo("[Resource] Successfully Loaded New Image: " + filePath);
				}
				return textureResource;
			}
			std::cout << "[Resource] Successfully Loaded New Image: " << filePath << std::endl;
			return textureResource;
		}
		//If the resource is not created and stored in cache properly, return nullptr
		if (m_logElementAttached)
		{
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Resource] Failed to Load New Image: " + filePath);
			}
			return nullptr;
		}
		std::cerr << "[Resource] Failed to Load New Image: " << filePath << std::endl;
		return nullptr;
	}

    //Load audio resource from a file path, returns shared pointer to AudioResource

	//Load a shader from a file path, returns a ShaderResource object
	std::shared_ptr<ShaderResource> ResourceElement::loadShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		//Check if the shader is already in the cache
		std::string combinedPath = vertexPath + "_" + fragmentPath;
		auto it = m_resourceCache.find(combinedPath);
		if (it != m_resourceCache.end())
		{
			//Shader already in cache, increment refCount and return the resource
			it->second->refCount++;
			return std::static_pointer_cast<ShaderResource>(it->second);
		}

		//Create a ShaderResource object with the texture ID and other necessary information
		auto shaderResource = std::make_shared<ShaderResource>();
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (!corePtr->getShaderElement())
			{
				if (m_logElementAttached)
				{
					corePtr->getLogElement()->logError("[Resource] Failed to Create Shader: " + combinedPath);
					return nullptr;
				}
				std::cerr << "[Resource] Failed to Create Shader: " << combinedPath << std::endl;
				return nullptr;
			}
			shaderResource->shaderProgramID = corePtr->getShaderElement()->createShaderProgram(vertexPath, fragmentPath);
		}

		shaderResource->refCount = 1;

		//Insert the new resource into the cache
		m_resourceCache[combinedPath] = shaderResource;

		//If the resource is created and stored in cache properly, return the resource
		if (shaderResource)
		{
			if (m_logElementAttached)
			{
				auto corePtr = m_core.lock();
				if (corePtr)
				{
					corePtr->getLogElement()->logInfo("[Resource] Successfully Loaded New Shader");
				}
				return std::static_pointer_cast<ShaderResource>(shaderResource);
			}
			std::cout << "[Resource] Successfully Loaded New Shader: " << combinedPath << std::endl;
			return std::static_pointer_cast<ShaderResource>(shaderResource);
		}
		//If the resource is not created and stored in cache properly, return nullptr
		if (m_logElementAttached)
		{
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Resource] Failed to Load New Shader");
			}
			return nullptr;
		}
		std::cerr << "[Resource] Failed to Load New Shader: " << combinedPath << std::endl;
		return nullptr;
	}

	//Load a texture resource from a file path, returns a TextureResource object
	std::shared_ptr<TextureResource> ResourceElement::loadTextureResource(const std::string& filePath)
	{
		//Load the image
		std::shared_ptr<Resource> resource = loadImage(filePath);
		//If the image was loaded successfully, return a shared pointer to the TextureResource object
		if (resource && resource->type == Resource::ResourceType::Texture)
		{
			//Return the TextureResource object
			return std::static_pointer_cast<TextureResource>(resource);
		}
		//If the image was not loaded successfully, return nullptr
		if (m_logElementAttached)
		{
			auto corePtr = m_core.lock();
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Resource] Failed to Load Texture Resource: " + filePath);
			}
			return nullptr;
		}
		std::cerr << "[Resource] Failed to Load Texture Resource: " << filePath << std::endl;
		return nullptr;
	}

	//Unload a resource from the cache
	void ResourceElement::unloadResource(const std::string& filePath) //If Shader resource, filePath should be std::string combinedPath = vertexPath + "_" + fragmentPath;
	{
		//Check if the resource is already in the cache
		auto it = m_resourceCache.find(filePath);
		//If the resource is in the cache, decrement the refCount
		if (it != m_resourceCache.end()) {
			//Decrement the refCount
			it->second->refCount--;
			//If the refCount is 0, delete the resource
			if (it->second->refCount <= 0) {
				//Delete the texture, if resource is a texture
				if (it->second->type == Resource::ResourceType::Texture)
				{
					//Delete the texture
					auto textureResource = std::static_pointer_cast<TextureResource>(it->second);
					glDeleteTextures(1, &textureResource->textureID);
				}
				//Delete the shader, if resource is a shader
				else if (it->second->type == Resource::ResourceType::Shader) {
					//Delete the shader program
					auto shaderResource = std::static_pointer_cast<ShaderResource>(it->second);
					glDeleteProgram(shaderResource->shaderProgramID);
				}
				//Remove resource from cache
				m_resourceCache.erase(it);
			}
		}
	}

	//Clear all resources from the cache
	void ResourceElement::clearCache()
	{
		//While there is still elements in the cache, unload resource data for each resource
		while (!m_resourceCache.empty())
		{
			unloadResource(m_resourceCache.begin()->first);
		}
		//Clear unordered map
		m_resourceCache.clear();
	}

	void ResourceElement::terminate()
	{
		clearCache();
	}
}