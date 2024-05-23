#include <string>
#include <utility>
#include <memory>
#include <cstdint>
#include <cctype>
#include <vector>
#include <map>
#include <iostream>
#include "TokenViewer.h"

class Node
{
public:
    virtual double Evaluate() = 0;
};

class EmptyNode : public Node
{
public:
    double Evaluate() override
    {
        throw std::runtime_error("cannot evaluate empty node!");
    }
};

class ConstantNode : public Node
{
    double value_ = 0.0;

public:
    double Evaluate() override
    {
        return this->value_;
    }

    explicit ConstantNode(double value) : value_(value) {}
};

class BinaryNode : public Node
{
    std::unique_ptr<Node> lhs_;
    std::unique_ptr<Node> rhs_;
    char operation_;

public:
    double Evaluate() override
    {
        switch(this->operation_)
        {
            case '+':
            {
                return this->lhs_->Evaluate() + this->rhs_->Evaluate();
            }

            case '-':
            {
                return this->lhs_->Evaluate() - this->rhs_->Evaluate();
            }

            case '*':
            {
                return this->lhs_->Evaluate() * this->rhs_->Evaluate();
            }

            case '/':
            {
                return this->lhs_->Evaluate() / this->rhs_->Evaluate();
            }
        }

        throw std::runtime_error("cannot perform unknown operation!");
    }

    BinaryNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs, char operation) : lhs_(std::move(lhs)), rhs_(std::move(rhs)), operation_(operation) {}
};

class ParentNode : public Node
{
    std::unique_ptr<Node> child_;

public:
    double Evaluate() override
    {
        return this->child_->Evaluate();
    }
};

enum class Precedence
{
    Max
};

class Expression {
    std::string raw_;
    std::unique_ptr<Node> tree_;

    TokenViewer token_viewer_;

    std::map<std::string, std::uint64_t> op_precedence_ = {
            {"+", 1},
            {"-", 1},
            {"*", 2},
            {"/", 2},
    };

    /*
     * Grammar
     * expression:
     *      | NUMBER
     *      | identifier
     *      | expression BINOP expression
     *      | '(' expression ')'
     *
     * EXAMPLES:
     * - 2 + 2
     * - 4^5
     * - 3 + 4 * 6
     * - x - 2
     */

    std::unique_ptr<Node> ParseOperation(std::unique_ptr<Node> lhs, Precedence precedence)
    {

    }

    std::unique_ptr<Node> ParseExpression(Precedence precedence = Precedence::Max)
    {
        auto t = this->token_viewer_.Consume();
        std::unique_ptr<Node> t_node;
        switch(t.type)
        {
            case TokenType::Number:
            {
                t_node = std::make_unique<ConstantNode>(t.As<double>());
                break;
            }

            default:
            {
                throw std::runtime_error("unexpected token!");
            }
        }

        while(!this->token_viewer_.Expect(TokenType::Terminator))
        {
            auto next = this->token_viewer_.Consume();

            switch(next.type)
            {
                case
            }
        }

        // Eat final ';'
        this->token_viewer_.Assert(TokenType::Terminator, ';');
        this->token_viewer_.Consume();
    }

public:
    double Evaluate(std::map<std::string, double> const &arguments = {})
    {
        return this->tree_->Evaluate();
    }

    explicit Expression(std::string equation) : raw_(std::move(equation)), token_viewer_(raw_)
    {
        this->tree_ = this->ParseExpression();
    }
};

int main()
{
    Expression eq("400 + 300 * 12");

    std::cout << eq.Evaluate() << std::endl;

    return 0;
}
