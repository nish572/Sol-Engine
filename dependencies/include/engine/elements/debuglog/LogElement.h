#pragma once

#include "EngineAPI.h"

//C++ libraries
#include <memory>
#include <iostream>
#include <string>

#pragma warning(push)
#pragma warning(disable: 6285 26450 26451 26437 26498 26800 26495) //Disable numerous spdlog warnings - warnings considered, spdlog fully functional and trusted, no need for these warnings

//The external library spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

//Forward declaration of Core class
//Essentially, telling compiler Sol::Core exists, but not providing full definition
//This means pointers to Core class can be used without including Core.h
//Including Core.h would introduce circular dependency
namespace Sol {
	class Core;
}

namespace CoreLogElement
{
	//Represents the element responsible for managing the logging operations in the Sol Engine
	//Logs will be output to a file
	class LogElement
	{
	public:
		LogElement(std::shared_ptr<Sol::Core> core);
		~LogElement();

		//Create file sink and set file to output to (creates file if it doesn't exist)
		//Set flush frequency/level
		//Create and register logger
		//Takes custom logfileName e.g. 'Sol-Log.txt'
		ENGINE_API bool initialize(const std::string& logfileName);

		//Use logger to log messages (info or error)
		void logInfo(const std::string& msg);
		void logError(const std::string& msg);

		//Use spdlog's shutdown function on the logger to clean up logger resources
		void terminate();

	private:
		//Pointer to core
		std::weak_ptr<Sol::Core> m_core;
		//Logger to store logger instance
		//spdlog requires shared_ptr as opposed to unique_ptr
		std::shared_ptr<spdlog::logger> logger;
	};
}