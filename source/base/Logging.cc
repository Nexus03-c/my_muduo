#include "Logging.h"
#include "TimeStamp.h"


Logger::OutputFunc g_output = nullptr;
static Logger::LogLevel g_log_level = Logger::LogLevel::INFO;

Logger::Logger(std::string base_name, int line): base_name_(base_name), line_(line), log_level_(Logger::LogLevel::INFO), stream_(){

}

Logger::Logger(std::string base_name, int line, Logger::LogLevel log_level): base_name_(base_name), line_(line), log_level_(log_level), stream_(){

}

Logger::~Logger() {
    stream_<<TimeStamp::now().toString();
    stream_<<" - "<<base_name_<<" : "<<line_;
    const LogStream::Buffer &buffer = stream_.buffer();
    g_output(buffer.data(), buffer.length());
}

void Logger::setOutput(OutputFunc func) { g_output = func;}
void Logger::setLogLevel(LogLevel log_level) { g_log_level = log_level;}
Logger::LogLevel Logger::log_level() { return g_log_level;}