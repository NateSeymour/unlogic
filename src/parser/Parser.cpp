#include "Parser.h"
#include "Node.h"

using namespace unlogic;

using G = ParserGrammarType;
using ValueType = ParserValueType;

bf::DefineTerminal<G, R"(given)"> KW_GIVEN(SyntaxKeyword);
bf::DefineTerminal<G, R"(calc)"> KW_CALC(SyntaxKeyword);
bf::DefineTerminal<G, R"(plot)"> KW_PLOT(SyntaxKeyword);

bf::DefineTerminal<G, R"(on)"> KW_ON(SyntaxKeyword);
bf::DefineTerminal<G, R"(as)"> KW_AS(SyntaxKeyword);

bf::DefineTerminal<G, R"(\d+(\.\d+)?)", double> NUMBER(SyntaxLiteral, [](auto const &tok) -> ValueType {
    return std::stod(std::string(tok.raw));
});

bf::DefineTerminal<G, R"([a-zA-Z]+)", std::string> IDENTIFIER(SyntaxIdentifier, [](auto const &tok) -> ValueType {
    return std::string(tok.raw);
});

bf::DefineTerminal<G, R"(\^)"> OP_EXP(bf::Right, SyntaxOperator);

bf::DefineTerminal<G, R"(\*)"> OP_MUL(bf::Left, SyntaxOperator);
bf::DefineTerminal<G, R"(\/)"> OP_DIV(bf::Left, SyntaxOperator);
bf::DefineTerminal<G, R"(\+)"> OP_ADD(bf::Left, SyntaxOperator);
bf::DefineTerminal<G, R"(\-)"> OP_SUB(bf::Left, SyntaxOperator);

bf::DefineTerminal<G, R"(=)"> OP_ASN(bf::Left, SyntaxOperator);

bf::DefineTerminal<G, R"(\()"> PAR_OPEN(SyntaxGrouper);
bf::DefineTerminal<G, R"(\))"> PAR_CLOSE(SyntaxGrouper);

bf::DefineTerminal<G, R"(\[)"> BRK_OPEN(SyntaxGrouper);
bf::DefineTerminal<G, R"(\])"> BRK_CLOSE(SyntaxGrouper);

bf::DefineTerminal<G, R"(;)"> STMT_DELIMITER(SyntaxOperator);

bf::DefineTerminal<G, R"(,)"> SEPARATOR(SyntaxOperator);

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

extern bf::DefineNonTerminal<G, std::unique_ptr<Node>> expression;

bf::DefineNonTerminal<G, std::vector<std::unique_ptr<Node>>> expression_list
    = bf::PR<G>(expression)<=>[](auto &$) -> ValueType
    {
        std::vector<std::unique_ptr<Node>> list;
        list.push_back(std::move(expression($[0])));

        return std::move(list);
    }
    | (expression_list + SEPARATOR + expression)<=>[](auto &$) -> ValueType
    {
        auto list = expression_list($[0]);
        list.push_back(std::move(expression($[2])));

        return std::move(list);
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> function_call
    = (IDENTIFIER + PAR_OPEN + expression_list + PAR_CLOSE)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<CallNode>(IDENTIFIER($[0]), std::move(expression_list($[2])));
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> expression
    = bf::PR<G>(NUMBER)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<NumericLiteralNode>(NUMBER($[0]));
    }
    | bf::PR<G>(IDENTIFIER)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<VariableNode>(IDENTIFIER($[0]));
    }
    | bf::PR<G>(function_call)<=>[](auto &$) -> ValueType
    {
        return std::move($[0]);
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

bf::DefineNonTerminal<G, std::unique_ptr<Node>> function_definition
    = (KW_GIVEN + IDENTIFIER + PAR_OPEN + identifier_list + PAR_CLOSE + OP_ASN + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<FunctionDefinitionNode>(
                std::move(IDENTIFIER($[1])),
                std::move(identifier_list($[3])),
                std::move(expression($[6]))
        );
    }
    | (KW_GIVEN + IDENTIFIER + PAR_OPEN + PAR_CLOSE + OP_ASN + expression)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<FunctionDefinitionNode>(
                std::move(IDENTIFIER($[1])),
                std::vector<std::string>{},
                std::move(expression($[5]))
        );
    }
    ;

bf::DefineNonTerminal<G> variable_definition
    = (KW_GIVEN + IDENTIFIER + OP_ASN + expression)
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> plot_command
    = (KW_PLOT + IDENTIFIER)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<PlotCommandNode>(IDENTIFIER($[1]));
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> statement
    = (function_definition + STMT_DELIMITER)<=>[](auto &$) -> ValueType
    {
        return std::move($[0]);
    }
    | (plot_command + STMT_DELIMITER)<=>[](auto &$) -> ValueType
    {
        return std::move($[0]);
    }
    ;

bf::DefineNonTerminal<G, std::vector<std::unique_ptr<Node>>> statement_list
    = bf::PR<G>(statement)<=>[](auto &$) -> ValueType
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
    }
    ;

bf::DefineNonTerminal<G, std::unique_ptr<Node>> program
    = bf::PR<G>(scoped_block)<=>[](auto &$) -> ValueType
    {
        return std::make_unique<ProgramEntryNode>(std::move(scoped_block($[0])));
    }
    ;

/*
 * Exports
 */
bf::NonTerminal<G> &unlogic::unlogic_program = program;
