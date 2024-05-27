#include "Node.h"

double EmptyNode::Evaluate(EvaluationContext &context)
{
    throw std::runtime_error("cannot evaluate empty node!");
}

double ConstantNode::Evaluate(EvaluationContext &context)
{
     return this->value_;
}

double BinaryNode::Evaluate(EvaluationContext &context)
{
    switch(this->operation_)
    {
        case '+':
        {
            return this->lhs_->Evaluate(context) + this->rhs_->Evaluate(context);
        }

        case '-':
        {
            return this->lhs_->Evaluate(context) - this->rhs_->Evaluate(context);
        }

        case '*':
        {
            return this->lhs_->Evaluate(context) * this->rhs_->Evaluate(context);
        }

        case '/':
        {
            return this->lhs_->Evaluate(context) / this->rhs_->Evaluate(context);
        }

        case '^':
        {
            return pow(this->lhs_->Evaluate(context), this->rhs_->Evaluate(context));
        }
    }

    throw std::runtime_error("cannot perform unknown operation!");
}

double BlockNode::Evaluate(EvaluationContext &context)
{
    double result = 0.0;

    for(auto &statement : this->statements_)
    {
        result = statement->Evaluate(context);
    }

    return result;
}

void BlockNode::AddStatement(std::unique_ptr<Node> statement)
{
    this->statements_.push_back(std::move(statement));
}

double FunctionDefinitionNode::Evaluate(EvaluationContext &context)
{
    context.functions[this->identifier_] = std::make_shared<Function>(this->parameters_, this->body_);

    return 0.0;
}

double IdentifierNode::Evaluate(EvaluationContext &context)
{
    return context.parameters[this->identifier_];
}

double FunctionCallNode::Evaluate(EvaluationContext &context)
{
    if(!context.functions.contains(this->identifier_))
    {
        throw std::runtime_error("attempted to call function that is not defined!");
    }

    return context.functions[this->identifier_]->Run(context, this->parameters_);
}
