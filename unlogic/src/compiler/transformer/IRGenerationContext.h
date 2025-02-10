#ifndef UNLOGIC_IRGENERATIONCONTEXT_H
#define UNLOGIC_IRGENERATIONCONTEXT_H

#include <city/Value.h>
#include "../Scope.h"

namespace unlogic
{
    struct IRGenerationContext
    {
        city::IRModule &module;
        Scope &scope;
        std::unordered_map<std::string, city::Function *> &functions;
    };
} // namespace unlogic

#endif // UNLOGIC_IRGENERATIONCONTEXT_H
