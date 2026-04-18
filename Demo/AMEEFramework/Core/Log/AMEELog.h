#pragma once
#include <cstdarg>
#include <cstdio>
#include <string>

namespace AMEE {

enum class LogLevel : uint8_t {
    Debug,
    Info,
    Warning,
    Error,
};

class Logger {
public:
    // Initialize logger (call once at startup)
    static void init(LogLevel minLevel = LogLevel::Debug);
    
    // Set minimum log level
    static void setLevel(LogLevel level);
    
    // Get minimum log level
    static LogLevel getLevel();
    
    // Log a message
    static void log(LogLevel level, const char* tag, const char* fmt, ...);
    
    // Log with va_list
    static void vlog(LogLevel level, const char* tag, const char* fmt, va_list args);
    
    // Enable/disable file logging
    static void enableFileLog(const char* path);
    static void disableFileLog();
    
    // Flush
    static void flush();
};

} // namespace AMEE

// Convenience macros
#define AMEE_LOG_DEBUG(tag, fmt, ...) AMEE::Logger::log(amee::LogLevel::Debug, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_INFO(tag, fmt, ...)  AMEE::Logger::log(amee::LogLevel::Info, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_WARN(tag, fmt, ...)  AMEE::Logger::log(amee::LogLevel::Warning, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_ERROR(tag, fmt, ...) AMEE::Logger::log(amee::LogLevel::Error, tag, fmt, ##__VA_ARGS__)
