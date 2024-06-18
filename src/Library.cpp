//
// Created by nathan on 6/18/24.
//

#include "Library.h"

unlogic::LibraryFunction::LibraryFunction(unlogic::Library &library, std::string name, void *function, std::uint8_t nargs)
{
    this->function = function;
    this->nargs = nargs;

    library.AddDefinition(name, this);
}
