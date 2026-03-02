#ifndef MY_MUDUO_ASYNCLOGGING_H
#define MY_MUDUO_ASYNCLOGGING_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

#include "nonecopyable.h"
#include "LogStream.h"

class AsyncLogging : public nonecopyable {
public:
    AsyncLogging(int flush_interval = 2, std::string log_file = "test.log");
    ~AsyncLogging();

    void append(const char *log_line, int len);
    void stop();
private:
    void threadFunc();
    const int flush_interval_;
    std::string log_file_;
    std::atomic<bool> running_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread logging_thread_;

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    BufferVector buffers_;
    BufferPtr current_buffer_;
    BufferPtr next_buffer_;
};
#endif