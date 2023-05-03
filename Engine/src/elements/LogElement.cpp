#include "debuglog/LogElement.h"

namespace CoreLogElement
{
	LogElement::LogElement(std::shared_ptr<Sol::Core> core)
	{
	}
	LogElement::~LogElement()
	{
	}

	bool LogElement::initialize(const std::string& logfileName)
	{
		try {
			//Create file sink to log messages to Sol-Log.txt
			//If file not present, file created
			//truncated on opening
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string(logfileName), true);
			//Create logger with the previously created file sink
			logger = std::make_shared<spdlog::logger>("SolLogger", file_sink);
			//Flush log file on errors to avoid losing error-level messages between buffers
			//If flushing is too frequent, performance hit, so limit to on error
			logger->flush_on(spdlog::level::err);
			//Register logger to spdlog
			spdlog::set_default_logger(logger);
			logger->info("[Log] Successfully Initialized Logger");
			return true;
		} catch (const std::exception& e) { //Catch any spdlog exceptions if spdlog throws
			//e.what() returns a C-style string that displays a description of the exception
			//std::cerr since logger not present
			std::cerr << e.what() << std::endl;
			return false;
		}
	}

	//Use logger to log messages appropriately (either info or error)
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