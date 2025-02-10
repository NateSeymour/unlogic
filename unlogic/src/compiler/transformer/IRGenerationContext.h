#ifndef UNLOGIC_IRGENERATIONCONTEXT_H
#define UNLOGIC_IRGENERATIONCONTEXT_H

#include "../Scope.h"

namespace unlogic
{
    struct IRGenerationContext
    {
        city::IRModule &module;
        Scope &scope;
    };
} // namespace unlogic

#endif // UNLOGIC_IRGENERATIONCONTEXT_H
