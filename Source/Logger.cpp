#include "Logger.hpp"

#include <spdlog/sinks/basic_file_sink.h> 
#include <spdlog/sinks/stdout_color_sinks.h> 

SharedPointer<spdlog::logger> Logger::_logger;

void Logger::Initialize()
{
	spdlog::set_pattern("[%s::%#] [%^%l%$]: %v");

	_logger = spdlog::stdout_color_st("Logger");
	_logger->set_level(spdlog::level::trace);

	spdlog::set_default_logger(_logger);
}