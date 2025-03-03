#pragma once

#include <vector>
#include <atomic>
#include <cstring>

namespace ptm {

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity) 
        : buffer_(capacity)
        , capacity_(capacity)
        , writeIndex_(0)
        , readIndex_(0) {}

    // Write data to the ring buffer
    // Returns number of elements actually written
    size_t write(const T* data, size_t count) {
        size_t available = capacity_ - (writeIndex_ - readIndex_.load(std::memory_order_acquire));
        size_t toWrite = std::min(count, available);
        
        if (toWrite == 0) return 0;

        size_t writePos = writeIndex_ % capacity_;
        size_t firstPart = std::min(toWrite, capacity_ - writePos);
        
        // Write first part
        std::memcpy(&buffer_[writePos], data, firstPart * sizeof(T));
        
        // Write second part if wrapping around
        if (firstPart < toWrite) {
            std::memcpy(&buffer_[0], data + firstPart, (toWrite - firstPart) * sizeof(T));
        }
        
        writeIndex_ += toWrite;
        return toWrite;
    }

    // Read data from the ring buffer
    // Returns number of elements actually read
    size_t read(T* data, size_t count) {
        size_t available = writeIndex_.load(std::memory_order_acquire) - readIndex_;
        size_t toRead = std::min(count, available);
        
        if (toRead == 0) return 0;

        size_t readPos = readIndex_ % capacity_;
        size_t firstPart = std::min(toRead, capacity_ - readPos);
        
        // Read first part
        std::memcpy(data, &buffer_[readPos], firstPart * sizeof(T));
        
        // Read second part if wrapping around
        if (firstPart < toRead) {
            std::memcpy(data + firstPart, &buffer_[0], (toRead - firstPart) * sizeof(T));
        }
        
        readIndex_ += toRead;
        return toRead;
    }

    // Get number of elements available for reading
    size_t available() const {
        return writeIndex_.load(std::memory_order_acquire) - 
               readIndex_.load(std::memory_order_acquire);
    }

    // Get free space available for writing
    size_t free() const {
        return capacity_ - available();
    }

    // Clear the buffer
    void clear() {
        readIndex_.store(0, std::memory_order_release);
        writeIndex_.store(0, std::memory_order_release);
    }

private:
    std::vector<T> buffer_;
    const size_t capacity_;
    std::atomic<size_t> writeIndex_;
    std::atomic<size_t> readIndex_;
};
} 