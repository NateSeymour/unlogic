#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <buffalo/buffalo.h>
#include <memory>
#include "Node.h"

namespace unlogic
{
    enum SyntaxHighlightingGroup
    {
        SyntaxOperator,
        SyntaxKeyword,
        SyntaxIdentifier,
        SyntaxLiteral,
        SyntaxGrouper,
    };

    using ParserValueType = std::variant<double, std::string, std::vector<std::string>, UniqueNode, std::vector<UniqueNode>>;
    using ParserGrammarType = bf::GrammarDefinition<ParserValueType, SyntaxHighlightingGroup>;

    extern bf::NonTerminal<ParserGrammarType> &unlogic_program;
} // namespace unlogic

#endif // UNLOGIC_PARSER_H
