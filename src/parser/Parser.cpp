#include "Parser.h"

using namespace unlogic;

using G = ParserGrammarType;
using ValueType = ParserValueType;

spex::CTRETokenizer<G> unlogic::tokenizer;

bf::DefineTerminal<G> OP_EXP = tokenizer.Terminal<R"(\^)", bf::Associativity::Right>();

bf::DefineTerminal<G> OP_MUL = tokenizer.Terminal<R"(\*)", bf::Associativity::Left>();
bf::DefineTerminal<G> OP_DIV = tokenizer.Terminal<R"(\/)", bf::Associativity::Left>();
bf::DefineTerminal<G> OP_ADD = tokenizer.Terminal<R"(\+)", bf::Associativity::Left>();
bf::DefineTerminal<G> OP_SUB = tokenizer.Terminal<R"(\-)", bf::Associativity::Left>();

bf::DefineTerminal<G> OP_ASN = tokenizer.Terminal<R"(:=)", bf::Associativity::Left>();

bf::DefineTerminal<G> PAR_OPEN = tokenizer.Terminal<R"(\()">();
bf::DefineTerminal<G> PAR_CLOSE = tokenizer.Terminal<R"(\))">();

bf::DefineTerminal<G> BRK_OPEN = tokenizer.Terminal<R"(\[)">();
bf::DefineTerminal<G> BRK_CLOSE = tokenizer.Terminal<R"(\])">();

bf::DefineTerminal<G> STMT_DELIMITER = tokenizer.Terminal<R"(;)">();

bf::DefineTerminal<G> SEPARATOR = tokenizer.Terminal<R"(,)">();

bf::DefineTerminal<G> KW_GIVEN = tokenizer.Terminal<R"(given)">();
bf::DefineTerminal<G> KW_CALC = tokenizer.Terminal<R"(calc)">();
bf::DefineTerminal<G> KW_PLOT = tokenizer.Terminal<R"(plot)">();

bf::DefineTerminal<G> KW_ON = tokenizer.Terminal<R"(on)">();
bf::DefineTerminal<G> KW_AS = tokenizer.Terminal<R"(as)">();

bf::DefineTerminal<G, double> NUMBER = tokenizer.Terminal<R"(\d+(\.\d+)?)">([](auto const &tok) -> ValueType {
    return std::stod(std::string(tok.raw));
});

bf::DefineTerminal<G, std::string> IDENTIFIER = tokenizer.Terminal<R"([a-zA-Z]+)">([](auto const &tok) -> ValueType {
    return std::string(tok.raw);
});

bf::DefineNonTerminal<G, std::unique_ptr<Node>> expression
    = bf::PR<G>(NUMBER)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<NumericLiteralNode>(NUMBER($[0]));
    }
    | bf::PR<G>(IDENTIFIER)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<StringLiteralNode>(IDENTIFIER($[0]));
    }
    | (PAR_OPEN + expression + PAR_CLOSE)<=>[](auto &$) -> ValueType
    {
        return std::move($[1]);
    }
    | (expression + OP_EXP + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<PotentiationNode>(std::move(expression($[0])), std::move(expression($[2])));
    }
    | (expression + OP_MUL + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<MultiplicationNode>(std::move(expression($[0])), std::move(expression($[2])));
    }
    | (expression + OP_DIV + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<DivisionNode>(std::move(expression($[0])), std::move(expression($[2])));
    }
    | (expression + OP_ADD + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<AdditionNode>(std::move(expression($[0])), std::move(expression($[2])));
    }
    | (expression + OP_SUB + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<SubtractionNode>(std::move(expression($[0])), std::move(expression($[2])));
    }
    ;

bf::DefineNonTerminal<G, std::vector<std::string>> identifier_list
    = bf::PR<G>(IDENTIFIER)<=>[](auto &$) -> ValueType
    {
        return std::vector<std::string>{ IDENTIFIER($[0]) };
    }
    | (identifier_list + SEPARATOR + IDENTIFIER)<=>[](auto &$) -> ValueType
    {
        auto list = identifier_list($[0]);
        list.push_back(IDENTIFIER($[2]));

        return std::move(list);
    }
    ;

bf::DefineNonTerminal<G, Prototype> function_definition
    = (KW_GIVEN + IDENTIFIER + PAR_OPEN + identifier_list + PAR_CLOSE + OP_ASN + expression)<=>[](auto &$) -> ValueType
    {
        return Prototype {
            .name = std::move(IDENTIFIER($[1])),
            .arguments = std::move(identifier_list($[3])),
            .body = std::move(expression($[6])),
        };
    }
    | (KW_GIVEN + IDENTIFIER + PAR_OPEN + PAR_CLOSE + OP_ASN + expression)<=>[](auto &$) -> ValueType
    {
        return Prototype {
            .name = std::move(IDENTIFIER($[1])),
            .body = std::move(expression($[5])),
        };
    }
    ;

bf::DefineNonTerminal<G> variable_definition
    = (KW_GIVEN + IDENTIFIER + OP_ASN + expression)
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> statement
    = bf::PR<G>(function_definition)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<FunctionDefinitionNode>(std::move(function_definition($[0])));
    }
    ;

bf::DefineNonTerminal<G, std::vector<std::unique_ptr<Node>>> statement_list
    = (statement + STMT_DELIMITER)<=>[](auto &$) -> ValueType
    {
        std::vector<std::unique_ptr<Node>> list;
        list.push_back(std::move(statement($[0])));

        return std::move(list);
    }
    | (statement_list + statement)<=>[](auto &$) -> ValueType
    {
        auto list = statement_list($[0]);
        list.push_back(std::move(statement($[1])));

        return std::move(list);
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> scoped_block
    = bf::PR<G>(statement_list)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<ScopedBlockNode>(std::move(statement_list($[0])));
    };

bf::Grammar<G> grammar(tokenizer, scoped_block);
bf::SLRParser<G> unlogic::parser(grammar);

std::map<bf::Terminal<G>, SyntaxHighlightingGroup> unlogic::syntax_highlighting_groups = {
        { OP_EXP, SyntaxHighlightingGroup::Operator },
        { OP_MUL, SyntaxHighlightingGroup::Operator },
        { OP_DIV, SyntaxHighlightingGroup::Operator },
        { OP_ADD, SyntaxHighlightingGroup::Operator },
        { OP_SUB, SyntaxHighlightingGroup::Operator },
        { OP_ASN, SyntaxHighlightingGroup::Operator },

        { KW_GIVEN, SyntaxHighlightingGroup::Keyword },
        { KW_CALC, SyntaxHighlightingGroup::Keyword },
        { KW_PLOT, SyntaxHighlightingGroup::Keyword },
};