#import "AMEELog.h"
#import <Foundation/Foundation.h>
#include <mutex>
#include <ctime>

namespace amee {

static LogLevel g_minLevel = LogLevel::Debug;
static FILE* g_fileLog = nullptr;
static std::mutex g_logMutex;

static const char* levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

static const char* levelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "\033[36m";   // Cyan
        case LogLevel::Info:    return "\033[32m";   // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error:   return "\033[31m";   // Red
    }
    return "\033[0m";
}

void Logger::init(LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_minLevel = minLevel;
    AMEE_LOG_INFO("Logger", "Logger initialized (level=%s)", levelToString(minLevel));
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_minLevel = level;
}

LogLevel Logger::getLevel() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    return g_minLevel;
}

void Logger::log(LogLevel level, const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(level, tag, fmt, args);
    va_end(args);
}

void Logger::vlog(LogLevel level, const char* tag, const char* fmt, va_list args) {
    if (level < g_minLevel) return;

    std::lock_guard<std::mutex> lock(g_logMutex);

    // Format timestamp
    time_t now = time(nullptr);
    struct tm* tm = localtime(&now);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm);

    // Format the user message
    char msg[1024];
    vsnprintf(msg, sizeof(msg), fmt, args);

    // Console output with color
    fprintf(stderr, "%s[%s] [%s] %s: %s\033[0m\n",
            levelColor(level), timeStr, levelToString(level), tag, msg);

    // File output (no color)
    if (g_fileLog) {
        fprintf(g_fileLog, "[%s] [%s] %s: %s\n",
                timeStr, levelToString(level), tag, msg);
        fflush(g_fileLog);
    }
}

void Logger::enableFileLog(const char* path) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_fileLog) {
        fclose(g_fileLog);
    }
    g_fileLog = fopen(path, "w");
    if (g_fileLog) {
        AMEE_LOG_INFO("Logger", "File logging enabled: %s", path);
    }
}

void Logger::disableFileLog() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_fileLog) {
        fclose(g_fileLog);
        g_fileLog = nullptr;
    }
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    fflush(stderr);
    if (g_fileLog) {
        fflush(g_fileLog);
    }
}

} // namespace amee
