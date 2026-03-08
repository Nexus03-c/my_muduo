set(base_SRCS
    AsyncLogging.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    TimeStamp.cc
    #TestAsyncLog.cc
)

#add_executable(aynclog_test ${base_SRCS})
add_library(moduo_base SHARED ${base_SRCS})