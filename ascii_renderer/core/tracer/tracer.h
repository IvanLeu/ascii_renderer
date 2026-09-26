#pragma once

#include "severity.h"
#include <filesystem>
#include <fstream>

namespace core
{

class Tracer
{
public:
    void Init(std::filesystem::path file_path, Severity severity);

    static Tracer& Get()
    {
        static Tracer instance;
        return instance;
    }

    void WriteTrace(const char* buffer, size_t size);
    bool ShouldTrace(Severity sev) const;

private:
    Tracer() = default;

private:
    bool initialized_ = false;
    std::filesystem::path file_path_;
    std::ofstream log_file_;
    Severity severity_ = Severity::NONE;
};

} // namespace core
