#include <iostream>

#include "MainApp.hpp"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

int main()
{
    MainApp app(SCR_WIDTH, SCR_HEIGHT);
    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
