#pragma once

#include "EngineAPI.h"

#include <memory>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace CoreLogElement
{
	//Represents the element responsible for managing the logging operations in the Sol Engine
	//Logs will be output to a file
	class LogElement
	{
	public:
		//Create log element
		LogElement();
		//Clean up log element resources
		~LogElement();

		//Create file sink and set file to output to (creates file if it doesn't exist)
		//Set flush frequency/level
		//Create logger
		//Register logger
		bool initialize();

		//Use logger to log messages appropriately
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