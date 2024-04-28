//------- Physics Element ---
//Manages Physics Simulations
//For The Sol Core Engine
//---------------------------

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

	//Call after Core's attachElement(elementName) has been called
	//Pass any required parameters for initialization, none
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

		//Set up gravity, current default is the Earth's gravity
		b2Vec2 gravity(0.0f, -9.81f);

		//Create the world with gravity
		m_world = std::make_shared<b2World>(gravity);

		//Enable continuous physics
		m_world->SetContinuousPhysics(true);

		//Enable auto-clearing of forces after each simulation step
		m_world->SetAutoClearForces(true);

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

	//Cleanup not inherently necessary as Box2D manages the world, but done so to ensure proper memory deallocation
	void PhysicsElement::terminate()
	{
		clearWorld();
		m_world = nullptr;
	}

	//Get the pointer to the world
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

	//Clear the world by destroying each body present in the world
	void PhysicsElement::clearWorld()
	{
		if (!m_world) return;

		//Iterate over the world's body list and destroy each body
		for (b2Body* body = m_world->GetBodyList(); body != nullptr;) {
			//Store the next body before destroying the current one
			b2Body* nextBody = body->GetNext();
			m_world->DestroyBody(body);
			body = nextBody;
		}
	}
}