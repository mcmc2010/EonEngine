#include "AMEELog.hpp"
#include <mutex>
#include <ctime>
#include <cstdio>
#include <cstdarg>

namespace AMEE {

static LogLevel g_MinLevel = LogLevel::Debug;
static FILE* g_pFileLog = nullptr;
static std::mutex g_LogMutex;

static const char* LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

static const char* LevelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "\033[36m";
        case LogLevel::Info:    return "\033[32m";
        case LogLevel::Warning: return "\033[33m";
        case LogLevel::Error:   return "\033[31m";
    }
    return "\033[0m";
}

void Logger::init(LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    g_MinLevel = minLevel;
    AMEE_LOG_INFO("Logger", "Logger initialized (level=%s)", LevelToString(minLevel));
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    g_MinLevel = level;
}

LogLevel Logger::getLevel() {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    return g_MinLevel;
}

void Logger::log(LogLevel level, const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(level, tag, fmt, args);
    va_end(args);
}

void Logger::vlog(LogLevel level, const char* tag, const char* fmt, va_list args) {
    if (level < g_MinLevel) return;

    std::lock_guard<std::mutex> lock(g_LogMutex);

    time_t now = time(nullptr);
    struct tm* tm = localtime(&now);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm);

    char msg[1024];
    vsnprintf(msg, sizeof(msg), fmt, args);

    fprintf(stderr, "%s[%s] [%s] %s: %s\033[0m\n",
            LevelColor(level), timeStr, LevelToString(level), tag, msg);

    if (g_pFileLog) {
        fprintf(g_pFileLog, "[%s] [%s] %s: %s\n",
                timeStr, LevelToString(level), tag, msg);
        fflush(g_pFileLog);
    }
}

void Logger::enableFileLog(const char* path) {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    if (g_pFileLog) {
        fclose(g_pFileLog);
    }
    g_pFileLog = fopen(path, "w");
    if (g_pFileLog) {
        AMEE_LOG_INFO("Logger", "File logging enabled: %s", path);
    }
}

void Logger::disableFileLog() {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    if (g_pFileLog) {
        fclose(g_pFileLog);
        g_pFileLog = nullptr;
    }
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(g_LogMutex);
    fflush(stderr);
    if (g_pFileLog) {
        fflush(g_pFileLog);
    }
}

} // namespace AMEE
