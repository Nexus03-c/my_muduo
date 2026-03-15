#ifndef MY_MUDUO_THREAD_H
#define MY_MUDUO_THREAD_H
#include "nonecopyable.h"
#include <functional>
#include <string>
#include <atomic>
#include <memory>
#include <thread>

class Thread : public nonecopyable {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc func, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started() const { return started_; }
    bool joined() const { return joined_; }
    const std::string& name() const { return name_; }

    static int thread_nums() { return thread_nums_; }
private:
    void SetDefaultName();
    bool started_;
    bool joined_;
    pid_t tid_;
    ThreadFunc func_;
    std::unique_ptr<std::thread> thread_;
    std::string name_;

    static std::atomic<int> thread_nums_;
};
#endif