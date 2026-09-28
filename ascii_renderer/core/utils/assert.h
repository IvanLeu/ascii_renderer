#pragma once

#include "../tracer/tracer_macros.h"
#include <assert.h>

#define ASSERT(expr)                                                           \
    if (!expr)                                                                 \
    {                                                                          \
        TRACE_ERROR() << "Assertion failed! Expression: " << #expr             \
                      << ". File : " << TRACE_CURRENT_FILE                     \
                      << ". Line: " << TRACE_CURRENT_LINE                      \
                      << ". Function: " << TRACE_CURRENT_FUNCTION;             \
        throw std::runtime_error("Assertion failed");                          \
    }