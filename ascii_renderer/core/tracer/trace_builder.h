#pragma once

#include "severity.h"
#include "tracer.h"
#include "../../utils/utility_classes.h"
#include <format>
#include <array>
#include <assert.h>

namespace core::tracer
{

struct TraceBuilder : utils::NonCopyable
{
    TraceBuilder(Tracer& tracer)
        : tracer_(tracer)
    {}

    ~TraceBuilder() 
    {
        buffer_[buffer_size_] = '\n';
        ++buffer_size_;
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
    std::array<char, buffer_max_size_> buffer_{};
    size_t buffer_size_ = 0u;
    Tracer& tracer_;
};

} // namespace core::tracer