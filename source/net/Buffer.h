#ifndef MY_MUDUO_BUFFER_H
#define MY_MUDUO_BUFFER_H
#include <vector>
#include <algorithm>
#include <string>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t init_size = kInitialSize)
      : buffer_(kCheapPrepend + init_size),
        reader_index_(kCheapPrepend),
        writer_index_(kCheapPrepend)
    {

    }

    size_t readableBytes() { return writer_index_ - reader_index_; }
    size_t writableBytes() { return buffer_.size() - writer_index_; }
    size_t prependableBytes() { return reader_index_; }

    const char* peek() const { return begin() + reader_index_; }

    const char* findCRLF() const{
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite()? nullptr : crlf;
    }

    void retrieveAll() {
        reader_index_ = kCheapPrepend;
        writer_index_ = kCheapPrepend;
    }
    
    void retrieve(size_t len) {
        if(len < readableBytes()) {
            reader_index_ += len;
        } else {
            retrieveAll();
        }
    }

    void retriveUntil(const char* end) {
        if(end<=beginWrite()) {
            retrieve(end-peek());
        }
    }
    
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        std::string res(peek(), len);
        retrieve(len);
        return res;
    }

    void ensureWritableBytes(size_t len) {
        if(writableBytes() < len) {
            makeSpace(len);
        }
    }

    void append(const char *data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len) {
        append(static_cast<const char*>(data), len);
    }

    void append(const std::string& str) {
        append(str.c_str(), str.size());
    }

    char* beginWrite() { return begin()+writer_index_; }
    const char* beginWrite() const { return begin()+writer_index_; }

    void hasWritten(size_t len) {
        writer_index_ += len;
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() { return &*buffer_.begin(); }
    const char* begin() const { return &*buffer_.begin();}

    void makeSpace(size_t len) {
        if(prependableBytes() + writableBytes() < len + kCheapPrepend) {
            buffer_.resize(writer_index_ + len);
        } else {
            size_t readable_bytes = readableBytes();
            std::copy(begin()+reader_index_, begin()+writer_index_, begin()+kCheapPrepend);
            reader_index_ = kCheapPrepend;
            writer_index_ = reader_index_ + readable_bytes;
        }
    }

    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;

    static const char kCRLF[];
};

#endif