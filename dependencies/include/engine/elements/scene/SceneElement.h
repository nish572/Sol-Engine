#pragma once

#include "EngineAPI.h"

#include <memory>
#include <string>

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreSceneElement
{
	//Represents the element responsible for managing the scene operations in the Sol Engine
	//This includes serialisation/deserialisation, scene loading/unloading, and scene ordering
	class SceneElement
	{
	public:
		//Initialize pointers
		SceneElement(std::shared_ptr<Sol::Core> core);
		//Release resources associated with the SceneElement instance
		~SceneElement();

		//Initialize SceneElement
		ENGINE_API bool initialize();

		//Terminate SceneElement
		void terminate();

	private:
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
	};
}