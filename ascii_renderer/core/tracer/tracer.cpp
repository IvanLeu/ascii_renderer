#include "tracer.h"
#include <assert.h>
#include <fstream>

namespace core
{

void Tracer::Init(std::filesystem::path file_path, Severity severity)
{
    file_path_ = std::move(file_path);
    severity_ = severity;
    log_file_.open(file_path_, std::ios_base::out | std::ios_base::trunc);
    initialized_ = true;
}

void Tracer::WriteTrace(const char* buffer, size_t size)
{
    assert(initialized_);
    log_file_.write(buffer, size);
}

bool Tracer::ShouldTrace(Severity sev) const 
{ 
    return sev >= severity_;
}

} // namespace core
