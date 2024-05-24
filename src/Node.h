#ifndef UNLOGIC_NODE_H
#define UNLOGIC_NODE_H

#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <ranges>

class Node;
class Function;

struct EvaluationContext
{
    std::map<std::string, double> parameters;
    std::map<std::string, std::shared_ptr<Function>> functions;
};

class Node
{
public:
    virtual double Evaluate(EvaluationContext &context) = 0;
    virtual ~Node() {}
};

class Function
{
    std::vector<std::string> parameters_;
    std::shared_ptr<Node> body_;

public:
    double Run(EvaluationContext &context, std::vector<std::unique_ptr<Node>> &parameter_values)
    {
        EvaluationContext scoped_context = context;
        for(auto const &[name, value] : std::views::zip(this->parameters_, parameter_values))
        {
            scoped_context.parameters[name] = value->Evaluate(context);
        }

        return this->body_->Evaluate(scoped_context);
    }

    Function(std::vector<std::string> parameters, std::shared_ptr<Node> body) : parameters_(std::move(parameters)), body_(std::move(body)) {}
};

class EmptyNode : public Node
{
public:
    double Evaluate(EvaluationContext &context) override;
};

class ConstantNode : public Node
{
    double value_ = 0.0;

public:
    double Evaluate(EvaluationContext &context) override;
    explicit ConstantNode(double value) : value_(value) {}
};

class BinaryNode : public Node
{
    std::unique_ptr<Node> lhs_;
    std::unique_ptr<Node> rhs_;
    char operation_;

public:
    double Evaluate(EvaluationContext &context) override;
    BinaryNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs, char operation) : lhs_(std::move(lhs)), rhs_(std::move(rhs)), operation_(operation) {}
};

class BlockNode : public Node
{
    std::vector<std::unique_ptr<Node>> statements_;

public:
    void AddStatement(std::unique_ptr<Node> statement);
    double Evaluate(EvaluationContext &context) override;
};

class FunctionDefinitionNode : public Node
{
    std::string identifier_;
    std::vector<std::string> parameters_;
    std::shared_ptr<Node> body_;

public:
    double Evaluate(EvaluationContext &context) override;
    FunctionDefinitionNode(std::string identifier, std::shared_ptr<Node> body, std::vector<std::string> parameters) : identifier_(std::move(identifier)), body_(std::move(body)), parameters_(std::move(parameters)) {}
};

class IdentifierNode : public Node
{
    std::string identifier_;

public:
    double Evaluate(EvaluationContext &context) override;
    IdentifierNode(std::string identifier) : identifier_(std::move(identifier)) {}
};

class FunctionCallNode : public Node
{
    std::string identifier_;
    std::vector<std::unique_ptr<Node>> parameters_;

public:
    double Evaluate(EvaluationContext &context) override;
    FunctionCallNode(std::string identifier, std::vector<std::unique_ptr<Node>> parameters) : identifier_(std::move(identifier)), parameters_(std::move(parameters)) {}
};


#endif //UNLOGIC_NODE_H
