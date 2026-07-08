
#include "application.h"

int main()
{
    Application app;

    if (!app.Initialize(1460, 810, "Face Shooting"))
    {
        return -1;
    }

    app.LoadAssets();

    app.Run();

    app.Shutdown();

    return 0;
}