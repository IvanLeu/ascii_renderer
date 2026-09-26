#include "application.h"
#include "utils\exception.h"
#include "core\tracer\tracer.h"
#include <format>

using namespace core;

void InitializeTracer()
{
    const auto log_path = std::filesystem::current_path().append("log\\app_log.log");
    Tracer::Get().Init(log_path, Severity::Debug);
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