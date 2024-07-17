#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <vector>
#include "parser/Lex.h"

namespace unlogic
{
    template<typename T, typename ValueType>
    class ProductionRule
    {
        std::vector<T> start_terminals_;
    };

    template<typename T, typename ValueType>
    class NonTerminal
    {
        std::vector<ProductionRule<T, ValueType>> production_rules_;
    };
}

#endif //UNLOGIC_PARSER_H
