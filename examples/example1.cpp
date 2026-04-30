#include "engine.h"
#include <stdio.h>

int main()
{
    auto window = WNE::Window::create(1920, 1080);

    if (!window)
        return 1;

    while (!window->isCloseRequested())
    {
        WNE::Engine::getInstance()->update();
    }

    return 0;
}