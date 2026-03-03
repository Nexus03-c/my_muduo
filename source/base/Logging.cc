#include "Logging.h"
#include "TimeStamp.h"


Logger::OutputFunc g_output = nullptr;
static Logger::LogLevel g_log_level = Logger::LogLevel::INFO;

Logger::Logger(std::string base_name, int line): line_(line), log_level_(Logger::LogLevel::INFO), stream_(){
    GetFileName(base_name);
    stream_<<TimeStamp::now().toString()<<" ";
}

Logger::Logger(std::string base_name, int line, Logger::LogLevel log_level): line_(line), log_level_(log_level), stream_(){
    GetFileName(base_name);
    stream_<<TimeStamp::now().toString()<<" ";
}

Logger::~Logger() {
    stream_<<" - "<<base_name_<<" : "<<line_<<'\n';
    const LogStream::Buffer &buffer = stream_.buffer();
    g_output(buffer.data(), buffer.length());
}

void Logger::GetFileName(std::string &base_name) {
    memset(base_name_, 0, 64);
    const char* s = strrchr(base_name.c_str(), '/') + 1;
    size_t offset = s - base_name.c_str();
    memcpy(base_name_, s, base_name.size() - offset);
}

void Logger::setOutput(OutputFunc func) { g_output = func;}
void Logger::setLogLevel(LogLevel log_level) { g_log_level = log_level;}
Logger::LogLevel Logger::log_level() { return g_log_level;}