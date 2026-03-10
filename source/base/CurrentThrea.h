#ifndef MY_MUDUO_CURRENTTHREAD_H
#define MY_MUDUO_CURRENTTHREAD_H

extern thread_local int t_cache_tid;

void cacheTid();

inline int tid() {
    if(__builtin_expect(t_cache_tid == 0, 0)) {
        cacheTid();
    }
    return t_cache_tid;
}

#endif