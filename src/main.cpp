#include "core/Engine.h"

int main(int argc, char* argv[])
{
    Engine engine;

    engine.init();

    while (engine.isRunning)
    {
        engine.step();
    };

    engine.destroy();
    return 0;
}
