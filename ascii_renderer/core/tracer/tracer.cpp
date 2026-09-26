#include "tracer.h"
#include <assert.h>
#include <fstream>

namespace core::tracer
{

void Tracer::Init(std::filesystem::path file_path, Severity severity)
{
    file_path_ = std::move(file_path);
    severity_ = severity;

    if (file_path_.has_parent_path())
    {
        std::error_code ec;
        std::filesystem::create_directories(file_path_.parent_path(), ec);
    }

    log_file_.open(file_path_, std::ios_base::out | std::ios_base::trunc);
    // Initialized only if file opening succeeded
    initialized_ = log_file_.is_open();
}

void Tracer::WriteTrace(const char* buffer, size_t size)
{
    try
    {
        if (initialized_)
        {
            log_file_.write(buffer, size);
        }
    }
    catch (...)
    {
        // Tracer silently fails
    }
}

bool Tracer::ShouldTrace(Severity sev) const 
{ 
    return sev >= severity_;
}

} // namespace core
