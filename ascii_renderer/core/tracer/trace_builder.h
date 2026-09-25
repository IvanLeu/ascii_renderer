#pragma once

#include "severity.h"
#include "tracer.h"
#include "../../utils/utility_classes.h"
#include <sstream>
#include <string>
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
        const auto str = buffer_.str();
        tracer_.WriteTrace(str.c_str(), str.size());
    }

    template<typename T>
    TraceBuilder& operator<<(const T& obj)
    {
        buffer_ << obj;
        return *this;
    }

private:
    Tracer& tracer_;
    std::stringstream buffer_;
};

} // namespace core::tracer