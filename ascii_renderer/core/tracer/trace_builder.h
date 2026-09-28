#pragma once

#include "severity.h"
#include "tracer.h"
#include "../../core/utils/utility_classes.h"
#include <format>
#include <array>
#include <chrono>

namespace core::tracer
{

struct TraceBuilder : utils::NonCopyable
{
    TraceBuilder(Tracer& tracer, Severity level)
        : tracer_(tracer)
    {
        const auto now = std::chrono::system_clock::now();
        const auto local_time = std::chrono::current_zone()->to_local(now);
        const auto time_ms =
            std::chrono::floor<std::chrono::milliseconds>(local_time);

        const auto buffer_data = buffer_.data();
        const auto [it, size] =
            std::format_to_n(buffer_data, buffer_max_size_ - buffer_size_,
                             "{:%H:%M:%S} [{}] ", time_ms, ToString(level));

        buffer_size_ += it - buffer_data;
    }

    ~TraceBuilder() 
    {
        if (buffer_size_ < buffer_max_size_)
        {
            buffer_[buffer_size_++] = '\n';
        }
        else
        {
            buffer_.back() = '\n';
        }
        tracer_.WriteTrace(buffer_.data(), buffer_size_);
    }

    template<typename T>
    TraceBuilder& operator<<(const T& obj)
    {
        const auto current_end = buffer_.data() + buffer_size_;
        const auto [it, size] = std::format_to_n(
            current_end, buffer_max_size_ - buffer_size_, "{}", obj);

        buffer_size_ += it - current_end;
        
        return *this;
    }

private:
    static constexpr size_t buffer_max_size_ = 512u;
    std::array<char, buffer_max_size_> buffer_;
    size_t buffer_size_ = 0u;
    Tracer& tracer_;
};

} // namespace core::tracer