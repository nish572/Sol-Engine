#pragma once

#include "EngineAPI.h"

#pragma warning(push)
#pragma warning(disable: 26495) //Disable numerous Box2D warnings - warnings considered, Box2D fully functional and trusted, no need for these warnings

//C++ libraries
#include <memory>
#include <string>

//External library Box2D
#include <Box2D/Box2D.h>
#pragma warning(pop)

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CorePhysicsElement
{
	//Represents the Element responsible for managing the physics (including collision detection and resolution, and force/impulse/torque applications) in the Sol Engine
	class PhysicsElement
	{
	public:
		PhysicsElement(std::shared_ptr<Sol::Core> core);
		~PhysicsElement();

		ENGINE_API bool initialize();

		//Resources for physics managed by Box2D, but world cleared and pointer to world set to nullptr to be safe
		void terminate();

		//Return a pointer to the Box2D world managed by the PhysicsElement/PhysicsSystem
		std::shared_ptr<b2World> getWorld() const;

		//Clear this world by removing all bodies and resetting the world
		void clearWorld();

	private:
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//Is LogElement present
		bool m_logElementAttached{ false };
		//Physics world
		std::shared_ptr<b2World> m_world;
	};
}