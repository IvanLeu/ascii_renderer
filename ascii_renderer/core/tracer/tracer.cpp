#include "tracer.h"
#include <fstream>

namespace core
{

Tracer g_tracer;

void Tracer::Init(std::filesystem::path file_path, Severity severity)
{
    file_path_ = std::move(file_path);
    severity_ = severity;
}

void Tracer::WriteTrace(const char* buffer, size_t size)
{
    std::ofstream file(file_path_);
    file.write(buffer, size);
}

Severity Tracer::GetSeverity() const { return severity_; }


} // namespace core
