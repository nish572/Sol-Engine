//#pragma once
//
//#include "EngineAPI.h"
//
//#include <memory>
//#include <string>
//
////Forward declaration of Core class
////Essentially, telling compiler Sol::Core exists, but not providing full definition
////This means pointers to Core class can be used without including Core.h
////Including Core.h would introduce circular dependency
//namespace Sol {
//	class Core;
//}
//
//namespace CorePhysicsElement
//{
//	//Represents the element responsible for managing the physics (including collisions) in the Sol Engine
//	class PhysicsElement
//	{
//	public:
//		//Initialize pointers ...
//		PhysicsElement(std::shared_ptr<Sol::Core> core);
//		//Release resources associated with the PhysicsElement instance
//		~PhysicsElement();
//
//		//Initialize PhysicsElement by ...
//		ENGINE_API bool initialize();
//
//		//Update PhysicsElement
//		void fixedUpdate(double deltaTime);
//
//		//Terminate PhysicsElement by .
//		void terminate();
//
//		//Return a pointer to the ... instance managed by the PhysicsElement
//		//ENGINE_API SDL_Window* get... const;
//
//	private:
//		//Pointer to core
//		std::weak_ptr<Sol::Core> m_core;
//		//Is LogElement present
//		bool m_logElementAttached{ false };
//	};
//}