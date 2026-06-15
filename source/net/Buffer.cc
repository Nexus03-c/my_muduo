#include "Buffer.h"
#include <sys/uio.h>
#include <errno.h>

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char extra_buffer[65536]; //额外栈上的64k
    struct iovec vec[2];
    const size_t writable_bytes = writableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable_bytes;
    vec[1].iov_base = extra_buffer;
    vec[1].iov_len = sizeof(extra_buffer);

    const int iov_cnt = (writable_bytes < sizeof(extra_buffer))? 2 : 1; //保证一次最少能读取64K
    const ssize_t n = ::readv(fd, vec, iov_cnt);
    if(n<0) {
        *savedErrno = errno;
    } else if(n < writable_bytes) { //只读取到了buffer_中
        writer_index_ += n;
    } else { //buffer_中写满了，部分读取到了extra_buffer中
        writer_index_ = buffer_.size();
        append(extra_buffer, n - writable_bytes);
    }
    return n;
}

const char Buffer::kCRLF[] = "\r\n";