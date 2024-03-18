#include "physics/PhysicsElement.h"

#include "Core.h"


namespace CorePhysicsElement
{
	PhysicsElement::PhysicsElement(std::shared_ptr<Sol::Core> core) : m_core(core)
	{
	}
	PhysicsElement::~PhysicsElement()
	{
	}

	bool PhysicsElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}

		//Set up gravity
		b2Vec2 gravity(0.0f, -9.81f);

		//Create the world with gravity
		m_world = std::make_shared<b2World>(gravity);

		//Enable continuous physics
		m_world->SetContinuousPhysics(true);

		//Enable auto-clearing of forces after each simulation step
		m_world->SetAutoClearForces(true);

		//Then log success and return true
		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Physics] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Physics] Successfully Initialized" << std::endl;
		return true;
	}

	void PhysicsElement::terminate()
	{
		//Cleanup first, so remove ...
		//Safe to call here as PhysicsElement terminated last
	}

	std::shared_ptr<b2World> PhysicsElement::getWorld() const
	{
		//If world is found, return m_world
		if (m_world)
		{
			return m_world;
		}

		//If no world found, log error and return nullptr
		auto corePtr = m_core.lock();
		if (m_logElementAttached)
		{
			//Get LogElement to log error
			if (corePtr)
			{
				corePtr->getLogElement()->logError("[Physics] Failed To Get World: nullptr found");
			}
		}
		std::cerr << "[Physics] Failed To Get World: nullptr found" << std::endl;
		return nullptr;
	}
}