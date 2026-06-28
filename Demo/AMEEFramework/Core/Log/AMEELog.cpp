#include "AMEELog.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>

namespace AMEE {

LogLevel Logger::gs_MinLevel = LogLevel::Debug;
FILE* Logger::gs_pFileLog = nullptr;
std::mutex Logger::gs_Mutex;
std::vector<Logger::LogEntry> Logger::gs_Buffer;

const char* Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

const char* Logger::levelColor(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:   return "\033[36m";
        case LogLevel::Info:    return "\033[32m";
        case LogLevel::Warning: return "\033[33m";
        case LogLevel::Error:   return "\033[31m";
    }
    return "\033[0m";
}

void Logger::init(LogLevel minLevel)
{
    gs_MinLevel = minLevel;
    log(LogLevel::Info, "Logger", "Logger initialized (level=%s)", levelToString(minLevel));
    flushAll();
}

void Logger::setLevel(LogLevel level)
{
    gs_MinLevel = level;
}

LogLevel Logger::getLevel()
{
    return gs_MinLevel;
}

void Logger::log(LogLevel level, const char* tag, const char* fmt, ...)
{
    if (level < gs_MinLevel) return;

    char msg[1024];
    va_list args;
    va_start(args, fmt);
    va_list argsCopy;
    va_copy(argsCopy, args);
    vsnprintf(msg, sizeof(msg), fmt, argsCopy);
    va_end(argsCopy);
    va_end(args);

    // scope: 释放锁后再 flush，避免 try_lock 死锁
    {
        LogEntry entry;
        entry.Level = level;
        strncpy(entry.Tag, tag, sizeof(entry.Tag) - 1);
        entry.Tag[sizeof(entry.Tag) - 1] = '\0';
        strncpy(entry.Message, msg, sizeof(entry.Message) - 1);
        entry.Message[sizeof(entry.Message) - 1] = '\0';

        std::lock_guard<std::mutex> lock(gs_Mutex);
        gs_Buffer.push_back(entry);
    }

    flush();
}

void Logger::flush()
{
    std::vector<LogEntry> flushBuffer;
    FILE* fileLog = nullptr;
    {
        std::unique_lock<std::mutex> lock(gs_Mutex, std::try_to_lock);
        if (!lock.owns_lock() || gs_Buffer.empty()) return;
        flushBuffer.swap(gs_Buffer);
        fileLog = gs_pFileLog;
    }
    writeEntries(flushBuffer, fileLog);
}

void Logger::flushAll()
{
    std::vector<LogEntry> flushBuffer;
    FILE* fileLog = nullptr;
    {
        std::lock_guard<std::mutex> lock(gs_Mutex);
        if (gs_Buffer.empty()) return;
        flushBuffer.swap(gs_Buffer);
        fileLog = gs_pFileLog;
    }
    writeEntries(flushBuffer, fileLog);
}

void Logger::writeEntries(const std::vector<LogEntry>& entries, FILE* fileLog)
{
    time_t now = time(nullptr);
    struct tm* tm = localtime(&now);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm);

    for (const auto& entry : entries) {
        fprintf(stderr, "%s[%s] [%s] %s: %s\033[0m\n",
                levelColor(entry.Level), timeStr,
                levelToString(entry.Level), entry.Tag, entry.Message);

        if (fileLog) {
            fprintf(fileLog, "[%s] [%s] %s: %s\n",
                    timeStr, levelToString(entry.Level),
                    entry.Tag, entry.Message);
        }
    }

    fflush(stderr);
    if (fileLog) {
        fflush(fileLog);
    }
}

void Logger::enableFileLog(const char* path)
{
    {
        std::lock_guard<std::mutex> lock(gs_Mutex);
        if (gs_pFileLog) {
            fclose(gs_pFileLog);
        }
        gs_pFileLog = fopen(path, "w");
    }
    if (gs_pFileLog) {
        log(LogLevel::Info, "Logger", "File logging enabled: %s", path);
    }
}

void Logger::disableFileLog()
{
    std::lock_guard<std::mutex> lock(gs_Mutex);
    if (gs_pFileLog) {
        fclose(gs_pFileLog);
        gs_pFileLog = nullptr;
    }
}

} // namespace AMEE
