#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <buffalo/buffalo.h>
#include <memory>

namespace unlogic
{
    class Node;

    enum SyntaxHighlightingGroup
    {
        SyntaxOperator,
        SyntaxKeyword,
        SyntaxIdentifier,
        SyntaxLiteral,
        SyntaxGrouper,
    };

    using ParserValueType = std::variant<double, std::string, std::vector<std::string>, std::vector<std::unique_ptr<Node>>, std::unique_ptr<Node>>;
    using ParserGrammarType = bf::GrammarDefinition<ParserValueType, SyntaxHighlightingGroup>;

    extern bf::NonTerminal<ParserGrammarType> &unlogic_program;
} // namespace unlogic

#endif // UNLOGIC_PARSER_H
