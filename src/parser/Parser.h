#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <buffalo/buffalo.h>
#include <buffalo/spex.h>
#include "Node.h"

namespace unlogic
{
    using ParserValueType = std::unique_ptr<Node>;
    using ParserGrammarType = bf::GrammarDefinition<ParserValueType>;

    extern spex::CTRETokenizer<ParserGrammarType> tokenizer;
    extern bf::SLRParser<ParserGrammarType> parser;
}

#endif //UNLOGIC_PARSER_H
