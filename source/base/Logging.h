#ifndef MY_MUDUO_LOGGING_H
#define MY_MUDUO_LOGGING_H
#include <string>
#include "nonecopyable.h"
#include "LogStream.h"


class Logger : public nonecopyable {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    typedef void (*OutputFunc)(const char* msg, int len);

    Logger(std::string base_name, int line);
    explicit Logger(std::string base_name, int line, LogLevel log_level);
    ~Logger();
    LogStream& stream() { return stream_;}
    static void setOutput(OutputFunc func);
    static void setLogLevel(LogLevel log_level);
    static LogLevel log_level();
private:
    void GetFileName(std::string &base_name);
    char base_name_[64];
    int line_;
    LogStream stream_;
    LogLevel log_level_;
};

#define LOG_DEBUG if(Logger::log_level() <= Logger::LogLevel::DEBUG) \
    Logger(__FILE__ , __LINE__, Logger::LogLevel::DEBUG).stream()
#define LOG_INFO if(Logger::log_level() <= Logger::LogLevel::INFO) \
    Logger(__FILE__ , __LINE__, Logger::LogLevel::INFO).stream()
#define LOG_WARNING Logger(__FILE__ , __LINE__, Logger::LogLevel::WARNING).stream()
#define LOG_ERROR Logger(__FILE__ , __LINE__, Logger::LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__ , __LINE__, Logger::LogLevel::FATAL).stream()

#endif