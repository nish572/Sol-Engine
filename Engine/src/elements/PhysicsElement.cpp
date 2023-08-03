//#include "physics/PhysicsElement.h"
//
//#include "Core.h"
//
//
//namespace CorePhysicsElement
//{
//	PhysicsElement::PhysicsElement(std::shared_ptr<Sol::Core> core) : m_core(core)
//	{
//	}
//	PhysicsElement::~PhysicsElement()
//	{
//	}
//
//	bool PhysicsElement::initialize()
//	{
//		auto corePtr = m_core.lock();
//		if (corePtr)
//		{
//			if (corePtr->getLogElement())
//			{
//				m_logElementAttached = true;
//			}
//		}
//
//		//If ...
//		//then log success and return true
//		if (m_logElementAttached)
//		{
//			if (corePtr)
//			{
//				corePtr->getLogElement()->logInfo("[Physics] Successfully Initialized");
//			}
//			return true;
//		}
//		std::cout << "[Physics] Successfully Initialized" << std::endl;
//		return true;
//	}
//
//	void PhysicsElement::fixedUpdate(double fixedTimstep)
//	{
//	}
//
//	void PhysicsElement::terminate()
//	{
//		//Cleanup first, so remove ...
//		//Safe to call here as PhysicsElement terminated last
//	}
//}