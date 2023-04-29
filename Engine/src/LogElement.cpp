#include "debuglog/LogElement.h"

namespace CoreLogElement
{
	LogElement::LogElement()
	{
	}
	LogElement::~LogElement()
	{
	}

	bool LogElement::initialize()
	{
		//Create file sink to log messages to Sol-Log.txt
		//If file not present, file created
		//truncated on opening
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Sol-Log.txt", true);
		//Create logger with the previously created file sink
		logger = std::make_shared<spdlog::logger>("SolLogger", file_sink);
		//Flush log file on errors to avoid losing error-level messages between buffers
		//If flushing is too frequent, performance hit, so limit to on error
		logger->flush_on(spdlog::level::err);
		//Register logger to spdlog
		spdlog::register_logger(logger);
		return true; //CHANGE
	}

	//Use logger to log messages appropriately
	void LogElement::logInfo(const std::string& msg)
	{
		logger->info(msg);
	}
	void LogElement::logError(const std::string& msg)
	{
		logger->error(msg);
	}

	void LogElement::terminate()
	{
		//Clean up logger resources
		spdlog::shutdown();
	}
}