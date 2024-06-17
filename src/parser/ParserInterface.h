#ifndef UNLOGIC_PARSERINTERFACE_H
#define UNLOGIC_PARSERINTERFACE_H

#include <string>
#include <vector>
#include "parser/Node.h"

namespace unlogic
{
    struct PrototypeSignature
    {
        std::string name;
        std::vector<std::string> arguments;
    };

    Prototype parse_prototype(std::string const &input);
}

#endif //UNLOGIC_PARSERINTERFACE_H
