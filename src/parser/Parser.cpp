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

bf::DefineTerminal<G, std::unique_ptr<Node>> NUMBER = tokenizer.Terminal<R"(\d+(\.\d+)?)">([](auto const &tok) -> ValueType {
    double value = std::stod(std::string(tok.raw));
    return std::make_unique<ConstantNode>(value);
});

bf::DefineTerminal<G, std::unique_ptr<VariableNode>> IDENTIFIER = tokenizer.Terminal<R"([a-zA-Z]+)">([](auto const &tok) -> ValueType {
    return std::make_unique<VariableNode>(std::string(tok.raw));
});

bf::DefineNonTerminal<G, std::unique_ptr<Node>> expression
    = bf::PR<G>(NUMBER)<=>[](auto &$) -> ValueType { return std::move($[0]); }
    | (PAR_OPEN + expression + PAR_CLOSE)<=>[](auto &$) -> ValueType { return std::move($[1]); }
    | (expression + OP_EXP + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<PotentiationNode>(std::move($[0]), std::move($[2]));
    }
    | (expression + OP_MUL + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<MultiplicationNode>(std::move($[0]), std::move($[2]));
    }
    | (expression + OP_DIV + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<DivisionNode>(std::move($[0]), std::move($[2]));
    }
    | (expression + OP_ADD + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<AdditionNode>(std::move($[0]), std::move($[2]));
    }
    | (expression + OP_SUB + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<SubtractionNode>(std::move($[0]), std::move($[2]));
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> statement
    = bf::PR<G>(expression)<=>[](auto &$) -> ValueType
    {
        return std::move($[0]);
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> statement_list
    = (statement + STMT_DELIMITER)<=>[](auto &$) -> ValueType
    {
        return std::move($[0]);
    }
    | (statement_list + statement)
    ;

bf::NonTerminal<G> program;

bf::Grammar<G> grammar(tokenizer, program);
bf::SLRParser<G> unlogic::parser(grammar);