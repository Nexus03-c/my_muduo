#include "CurrentThrea.h"
#include <unistd.h>
#include <sys/syscall.h>

thread_local int t_cache_tid = 0;

void cacheTid() {
    if(t_cache_tid == 0) {
        t_cache_tid = static_cast<pid_t>(syscall(SYS_gettid));
    }
}