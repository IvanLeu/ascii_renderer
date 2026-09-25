#pragma once

#include "severity.h"
#include <filesystem>

namespace core
{

class Tracer
{
public:
    void Init(std::filesystem::path file_path, Severity severity);
    void WriteTrace(const char* buffer, size_t size);
    Severity GetSeverity() const;

private:
    std::filesystem::path file_path_;
    Severity severity_;
};

extern Tracer g_tracer;

} // namespace core
