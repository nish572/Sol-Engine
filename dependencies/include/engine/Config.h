//------- Config ------------
//Project Settings Management
//For The Sol Core Engine
//---------------------------

#pragma once

#include "EngineAPI.h"

#pragma warning(push)
#pragma warning(disable: 4244)

//C++ libraries
#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#pragma warning(pop)

namespace ApplicationConfig
{
	class Config {
	public:
        //Global, public members for project settings
		static float screenWidth;
		static float screenHeight;

		static std::string projectName;
		static std::string projectPath;

        //Set functions to set these values
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

        //A function to allow saving these settings to Config.txt
        static void saveConfig() {
            std::string configFilePath = projectPath + "/Config.txt";
            std::ofstream file(configFilePath);
            if (!file) {
                std::cerr << "Error: Unable to open file for writing at " << configFilePath << std::endl;
                return;
            }
            file << "Project Name: " << projectName << std::endl;
            file << "Project Path: " << projectPath << std::endl;
            file << "Game Width: " << screenWidth << std::endl;
            file << "Game Height: " << screenHeight << std::endl;
            file.close();
        }

        //A function to allow loading these settings from Config.txt
        static void loadConfig() {
            std::string configFilePath = projectPath + "/Config.txt";
            std::ifstream file(configFilePath);
            if (!file) {
                std::cerr << "Error: Unable to open file for reading at " << configFilePath << std::endl;
                return;
            }
            std::string line;
            while (getline(file, line)) {
                if (line.find("Project Name: ") == 0) {
                    projectName = line.substr(14);
                }
                else if (line.find("Project Path: ") == 0) {
                    projectPath = line.substr(14);
                }
                else if (line.find("Game Width: ") == 0) {
                    screenWidth = std::stoi(line.substr(12));
                }
                else if (line.find("Game Height: ") == 0) {
                    screenHeight = std::stoi(line.substr(13));
                }
            }
            file.close();
        }
	};
}