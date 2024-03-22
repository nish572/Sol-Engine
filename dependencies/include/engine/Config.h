#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>
#include <string>

namespace ApplicationConfig
{
	class Config {
	public:
		static float screenWidth;
		static float screenHeight;

		static void setScreenSize(float width, float height) {
			screenWidth = width;
			screenHeight = height;
		}
	};
}