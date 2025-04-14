#include "Engine/Engine.h"

int main(int, char**)
{
    Engine::Engine* engine = new Engine::Engine();
    int exitCode = engine->Run();
    delete engine;
    return exitCode;
}
