#include "RuntimeLibrary.h"

using namespace unlogic;

extern "C"
{
    Runtime *unlogic_runtime_global = nullptr;

    void unlogic_runtime_init()
    {
        unlogic_runtime_global = new Runtime();
    }

    void unlogic_runtime_destroy()
    {
        delete unlogic_runtime_global;
    }
}

Library unlogic::runtime("runtime");

LibrarySymbol runtime_init(runtime, "runtime_init", (void*)unlogic_runtime_init);
LibrarySymbol runtime_destory(runtime, "runtime_destroy", (void*)unlogic_runtime_destroy);