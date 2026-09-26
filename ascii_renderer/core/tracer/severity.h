#pragma once

#include <string_view>
#include <assert.h>

namespace core::tracer
{

enum class Severity
{
    None,
    Debug,
    Info,
    Warning,
    Error
};

constexpr std::string_view ToString(Severity sev) noexcept
{
    switch (sev)
    {
    case Severity::Debug:
        return "DBG";
    case Severity::Info:
        return "INF";
    case Severity::Warning:
        return "WRN";
    case Severity::Error:
        return "ERR";
    }

    assert(false);
    return "";
}

}