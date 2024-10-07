#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <map>
#include <buffalo/buffalo.h>
#include <buffalo/spex.h>
#include "Node.h"

namespace unlogic
{
    using ParserValueType = std::variant<
            double,
            std::string,
            Prototype,
            std::vector<std::string>,
            std::vector<std::unique_ptr<Node>>,
            std::unique_ptr<Node>,
            Program
    >;
    using ParserGrammarType = bf::GrammarDefinition<ParserValueType>;

    extern spex::CTRETokenizer<ParserGrammarType> tokenizer;
    extern bf::SLRParser<ParserGrammarType> parser;

    enum class SyntaxHighlightingGroup
    {
        Operator,
        Keyword,
        Identifier,
    };

    extern std::map<bf::Terminal<ParserGrammarType>, SyntaxHighlightingGroup> syntax_highlighting_groups;
}

#endif //UNLOGIC_PARSER_H
