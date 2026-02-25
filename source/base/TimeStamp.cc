#include "TimeStamp.h"

TimeStamp TimeStamp::now() {
    return TimeStamp(time(NULL));
}

std::string TimeStamp::toString() {
    char buffer[128];
    tm* time_tm =  localtime(&microsecondsSinceEpoch_);
    snprintf(buffer, 128, "%04d/%02d/%02d %02d:%02d:%02d", time_tm->tm_year+1900, 
        time_tm->tm_mon+1,
        time_tm->tm_mday,
        time_tm->tm_hour,
        time_tm->tm_min,
        time_tm->tm_sec);
    return buffer;
}