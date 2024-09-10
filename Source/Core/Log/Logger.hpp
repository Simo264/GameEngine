#pragma once

#include "Core/Core.hpp"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

/* ----------------------------------------- 
			Static class Logger
	----------------------------------------- */
class Logger
{
public:
	Logger()	= delete;
	~Logger() = delete;

	static const SharedPtr<spdlog::logger>& GetLogger() { return _logger; }

	static void Initialize();

private:
	static SharedPtr<spdlog::logger> _logger;
};

#define CONSOLE_TRACE(...)		SPDLOG_LOGGER_TRACE(Logger::GetLogger(),		__VA_ARGS__) 
#define CONSOLE_DEBUG(...)		SPDLOG_LOGGER_DEBUG(Logger::GetLogger(),		__VA_ARGS__) 
#define CONSOLE_INFO(...)			SPDLOG_LOGGER_INFO(Logger::GetLogger(),			__VA_ARGS__) 
#define CONSOLE_WARN(...)			SPDLOG_LOGGER_WARN(Logger::GetLogger(),			__VA_ARGS__) 
#define CONSOLE_ERROR(...)		SPDLOG_LOGGER_ERROR(Logger::GetLogger(),		__VA_ARGS__) 
#define CONSOLE_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Logger::GetLogger(), __VA_ARGS__) 