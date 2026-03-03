#include "string"
#include "AsyncLogging.h"
#include "Logging.h"

AsyncLogging* log = nullptr;

void OutputFunc(const char *str, int len) {
    log->append(str, len);
}

int main() {
    log = new AsyncLogging();
    Logger::setOutput(OutputFunc);

    LOG_INFO << "Test log....";
    LOG_INFO << 123456;
    struct timespec ts = { 5, 0 };
    nanosleep(&ts, NULL);
    return 0;
}