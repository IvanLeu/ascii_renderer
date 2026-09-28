#include "application.h"
#include "core\utils\exception.h"
#include "core\tracer\tracer.h"
#include <format>

using namespace core;

void InitializeTracer()
{
    const auto now = std::chrono::system_clock::now();
    const auto local_time = std::chrono::current_zone()->to_local(now);
    const auto time_s =
        std::chrono::floor<std::chrono::seconds>(local_time);
    
    const auto log_filename = std::format("ascii_renderer_{:%d.%m.%Y_%H.%M.%S}.log", time_s);

    const auto log_path = std::filesystem::current_path().append("log").append(log_filename);
    tracer::Tracer::Get().Init(log_path, tracer::Severity::Info);
}

int main()
{
    using namespace core::utils;
    try
    {
        InitializeTracer();

        core::Application app;
        return app.Run();
    }
    catch (Exception& e)
    {
        MessageBox(NULL, e.what(),
                   std::format("An {} occured!", e.GetType()).c_str(), MB_OK);
    }
    catch (std::exception& e)
    {
        MessageBox(NULL, e.what(), "An exception occured!", MB_OK);
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown exception", "Unhandled exception occured!",
                   MB_OK);
    }

    return 0;
}