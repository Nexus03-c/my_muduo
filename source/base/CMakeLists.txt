set(base_SRCS
    AsyncLogging.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    TimeStamp.cc
    Thread.cc
    CurrentThread.cc
    #TestAsyncLog.cc
)

#add_executable(aynclog_test ${base_SRCS})
add_library(muduo_base SHARED ${base_SRCS})