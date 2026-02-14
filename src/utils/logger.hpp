#pragma once

#include <memory>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

class Logger
{
public:
	Logger()	= delete;
	~Logger() = delete;

	static void initialize();
	static const auto& get_logger() { return logger_; }

private:
	static inline std::shared_ptr<spdlog::logger> logger_;
};

#define CONSOLE_TRACE(...)		SPDLOG_LOGGER_TRACE(Logger::get_logger(),		__VA_ARGS__) 
#define CONSOLE_DEBUG(...)		SPDLOG_LOGGER_DEBUG(Logger::get_logger(),		__VA_ARGS__) 
#define CONSOLE_INFO(...)			SPDLOG_LOGGER_INFO(Logger::get_logger(),			__VA_ARGS__) 
#define CONSOLE_WARN(...)			SPDLOG_LOGGER_WARN(Logger::get_logger(),			__VA_ARGS__) 
#define CONSOLE_ERROR(...)		SPDLOG_LOGGER_ERROR(Logger::get_logger(),		__VA_ARGS__) 
#define CONSOLE_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Logger::get_logger(), __VA_ARGS__) 