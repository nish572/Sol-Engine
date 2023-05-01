#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>
#include <string>

#pragma warning(push)
#pragma warning(disable: 6285 26450 26451 26437 26498 26800 26495) //Disable numerous spdlog warnings - warnings considered, spdlog fully functional and trusted, no need for these warnings
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace CoreLogElement
{
	//Represents the element responsible for managing the logging operations in the Sol Engine
	//Logs will be output to a file
	class LogElement
	{
	public:
		//Create LogElement
		LogElement();
		//Clean up LogElement resources
		~LogElement();

		//Create file sink and set file to output to (creates file if it doesn't exist)
		//Set flush frequency/level
		//Create logger
		//Register logger
		//Takes custom logfileName e.g. 'Sol-Log.txt'
		bool initialize(const std::string& logfileName);

		//Use logger to log messages
		ENGINE_API void logInfo(const std::string& msg);
		ENGINE_API void logError(const std::string& msg);

		//Use spdlog's shutdown on the logger to clean up logger resources
		void terminate();

	private:
		//Logger to store logger instance
		//spdlog requires shared_ptr as opposed to unique_ptr
		std::shared_ptr<spdlog::logger> logger;
	};
}