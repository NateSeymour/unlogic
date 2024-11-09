#ifndef UNLOGIC_RUNTIMELIBRARY_H
#define UNLOGIC_RUNTIMELIBRARY_H

#include "compiler/Library.h"

namespace unlogic
{
    struct RuntimeResult
    {

    };

    struct Runtime
    {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        std::uint32_t center_x;
        std::uint32_t center_y;

        RuntimeResult result;
    };

    extern Library runtime;
}

#endif //UNLOGIC_RUNTIMELIBRARY_H