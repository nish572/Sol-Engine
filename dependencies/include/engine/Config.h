#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>
#include <string>
#include <fstream>

namespace ApplicationConfig
{
	class Config {
	public:
		static float screenWidth;
		static float screenHeight;

		static std::string projectName;
		static std::string projectPath;

		static void setScreenSize(float width, float height) {
			screenWidth = width;
			screenHeight = height;
		}

		static void setProjectName(std::string name) {
			projectName = name;
		}

		static void setProjectPath(std::string path) {
			projectPath = path;
		}

		static void saveConfig() {
			std::ofstream file("Config.txt");
			file << "Project Name: " << projectName << std::endl;
			file << "Project Path: " << projectPath << std::endl;
			file << "Game Width: " << screenWidth << std::endl;
			file << "Game Height: " << screenHeight << std::endl;
			file.close();
		}

		static void loadConfig() {
			std::ifstream file("Config.txt");
			std::string line;
			while (getline(file, line)) {
				if (line.find("Project Name: ") == 0) {
					projectName = line.substr(14);
				}
				else if (line.find("Project Path: ") == 0) {
					projectPath = line.substr(14);
				}
				else if (line.find("Game Width: ") == 0) {
					screenWidth = std::stof(line.substr(12));
				}
				else if (line.find("Game Height: ") == 0) {
					screenHeight = std::stof(line.substr(13));
				}
			}
			file.close();
		}
	};
}