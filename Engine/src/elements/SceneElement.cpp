#include "scene/SceneElement.h"

#include "Core.h"


namespace CoreSceneElement
{
	SceneElement::SceneElement(std::shared_ptr<Sol::Core> core) : m_core(core)
	{
	}
	SceneElement::~SceneElement()
	{
	}

	bool SceneElement::initialize()
	{
		auto corePtr = m_core.lock();
		if (corePtr)
		{
			if (corePtr->getLogElement())
			{
				m_logElementAttached = true;
			}
		}

		//Then log success and return true
		if (m_logElementAttached)
		{
			if (corePtr)
			{
				corePtr->getLogElement()->logInfo("[Scene] Successfully Initialized");
			}
			return true;
		}
		std::cout << "[Scene] Successfully Initialized" << std::endl;
		return true;
	}

	void SceneElement::terminate()
	{
		return;
	}
}