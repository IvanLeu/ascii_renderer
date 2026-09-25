#include "application.h"
#include "utils\exception.h"
#include <format>

int main()
{
    using namespace core::utils;
    try
    {
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