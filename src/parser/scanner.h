//
// Created by nathan on 6/13/24.
//

#ifndef UNLOGIC_SCANNER_H
#define UNLOGIC_SCANNER_H

#include <string>
#include <sstream>

#include "parser/Parser.hpp"

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer unlogic_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL unlogic::Parser::symbol_type unlogic::Scanner::get_next_token()

namespace unlogic
{
    class Scanner : public yyFlexLexer
    {
    private:
        std::stringstream stream;
    public:
        virtual unlogic::Parser::symbol_type get_next_token();
        virtual ~Scanner() {}

        Scanner(std::string const &input) : stream(input)
        {
            this->switch_streams(&this->stream, nullptr);
        }
    };
}

#endif //UNLOGIC_SCANNER_H
