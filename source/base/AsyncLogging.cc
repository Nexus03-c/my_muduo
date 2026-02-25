#include "AsyncLogging.h"
#include "LogFile.h"

AsyncLogging::AsyncLogging(int flush_interval): 
    flush_interval_(flush_interval),
    running_(true),
    current_buffer_(new Buffer),
    next_buffer_(new Buffer),
    logging_thread_(threadFunc)
{
    current_buffer_->bZero();
    next_buffer_->bZero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
    if(running_) {
        stop();
    }
}

void AsyncLogging::append(const char *log_line, int len) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(len < current_buffer_->avail()) {
        current_buffer_->append(log_line, len);
    } else {
        buffers_.push_back(std::move(current_buffer_));
        if(next_buffer_) {
            current_buffer_ = std::move(next_buffer_);
        } else {
            current_buffer_.reset(new Buffer());
        }
        current_buffer_->append(log_line, len);
        cond_.notify_all();
    }
}

void AsyncLogging::stop() {
    running_ = false;
    cond_.notify_all();
    logging_thread_.join();
}

void AsyncLogging::threadFunc() {
    BufferPtr new_buffer1(new Buffer());
    BufferPtr new_buffer2(new Buffer());
    new_buffer1->bZero();
    new_buffer2->bZero();
    BufferVector buffers_to_output;
    buffers_to_output.reserve(16);

    LogFile log_file;
    while(running_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty()) {
                cond_.wait_for(lock, std::chrono::seconds(3));
            }
            buffers_.push_back(std::move(current_buffer_));
            current_buffer_ = std::move(new_buffer1);
            buffers_to_output.swap(buffers_);
            if(!next_buffer_) {
                next_buffer_ = std::move(new_buffer2);
            }
        }

        for(const auto &buffer : buffers_to_output) {
            //TODO: log_file.append
        }

        if(buffers_to_output.size()>2) {
            buffers_to_output.resize(2);
        }

        if(!new_buffer1) {
            new_buffer1 = std::move(buffers_to_output.back());
            buffers_to_output.pop_back();
            new_buffer1->reset();
        }
        if(!new_buffer2) {
            new_buffer2 = std::move(buffers_to_output.back());
            buffers_to_output.pop_back();
            new_buffer2->reset();
        }
        buffers_to_output.clear();
        //TODO: flush
    }
    //TODO: flush
}