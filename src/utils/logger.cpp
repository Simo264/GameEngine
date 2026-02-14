#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h> 
#include <spdlog/sinks/stdout_color_sinks.h> 

void Logger::initialize()
{
	/* https://github.com/gabime/spdlog/wiki/3.-Custom-formatting */
	spdlog::set_pattern("[%s::%#] [%^%l%$]: %v");

	logger_ = spdlog::stdout_color_st("Logger");
	logger_->set_level(spdlog::level::trace);

	spdlog::set_default_logger(logger_);
}