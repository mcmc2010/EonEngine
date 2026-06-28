#ifndef __AMEE_LOG_H__
#define __AMEE_LOG_H__
#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>
#include <mutex>

namespace AMEE {

enum class LogLevel : uint8_t {
    Debug,
    Info,
    Warning,
    Error,
};

class Logger {
public:
    static void init(LogLevel minLevel = LogLevel::Debug);
    static void setLevel(LogLevel level);
    static LogLevel getLevel();

    // 格式化 + 入缓冲，不做 I/O，永远不阻塞
    static void log(LogLevel level, const char* tag, const char* fmt, ...);

    // 非阻塞 flush：try_lock 成功则写出全部缓冲，失败则跳过
    static void flush();

    // 阻塞 flush：强制写出，用于退出前
    static void flushAll();

    static void enableFileLog(const char* path);
    static void disableFileLog();

private:
    struct LogEntry {
        LogLevel Level;
        char Tag[32];
        char Message[1024];
    };

    static LogLevel gs_MinLevel;
    static FILE* gs_pFileLog;
    static std::mutex gs_Mutex;
    static std::vector<LogEntry> gs_Buffer;

    static void writeEntries(const std::vector<LogEntry>& entries, FILE* fileLog);

    static const char* levelToString(LogLevel level);
    static const char* levelColor(LogLevel level);
};

#define AMEE_LOG_DEBUG(tag, fmt, ...) AMEE::Logger::log(AMEE::LogLevel::Debug, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_INFO(tag, fmt, ...)  AMEE::Logger::log(AMEE::LogLevel::Info, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_WARN(tag, fmt, ...)  AMEE::Logger::log(AMEE::LogLevel::Warning, tag, fmt, ##__VA_ARGS__)
#define AMEE_LOG_ERROR(tag, fmt, ...) AMEE::Logger::log(AMEE::LogLevel::Error, tag, fmt, ##__VA_ARGS__)

} // namespace AMEE

#endif // __AMEE_LOG_H__
