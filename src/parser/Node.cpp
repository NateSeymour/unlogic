#include "Node.h"

using namespace unlogic;

std::vector<Node const *> VariableNode::Children() const
{
    return { this };
}

llvm::Value *VariableNode::Codegen(CompilationContext &ctx)
{
    return ctx.named_values[this->identifier_];
}

std::unique_ptr<Node> VariableNode::Derive() const
{
    return std::make_unique<ConstantNode>(1.0);
}

std::unique_ptr<Node> VariableNode::Copy() const
{
    return std::make_unique<VariableNode>(this->identifier_);
}

llvm::Value *ConstantNode::Codegen(CompilationContext &ctx)
{
    return llvm::ConstantFP::get(*ctx.llvm_ctx, llvm::APFloat(this->value_));
}

std::vector<const Node *> ConstantNode::Children() const
{
    return { this };
}

std::unique_ptr<Node> ConstantNode::Derive() const
{
    return std::make_unique<ConstantNode>(0);
}

std::unique_ptr<Node> ConstantNode::Copy() const
{
    return std::make_unique<ConstantNode>(this->value_);
}

llvm::Value *CallNode::Codegen(CompilationContext &ctx)
{
    llvm::Function *function = ctx.module->getFunction(this->function_name_);

    if(function->arg_size() < this->arguments_.size())
    {
        throw std::runtime_error("Aaaaaahhhhhhh");
    }

    std::vector<llvm::Value *> argument_values;
    argument_values.reserve(this->arguments_.size());
    for(auto const &argument : this->arguments_)
    {
        argument_values.push_back(argument->Codegen(ctx));
    }

    return ctx.builder->CreateCall(function, argument_values, "calltmp");
}

std::vector<const Node *> CallNode::Children() const
{
    std::vector<Node const *> children{this};
    for(auto const &argument : this->arguments_)
    {
        for(auto const &child : argument->Children())
        {
            children.push_back(child);
        }
    }

    return children;
}

std::unique_ptr<Node> CallNode::Derive() const {
    return std::unique_ptr<Node>();
}

std::unique_ptr<Node> CallNode::Copy() const {
    return std::unique_ptr<Node>();
}

std::unique_ptr<Node> AdditionNode::Derive() const
{
    return std::make_unique<AdditionNode>(this->lhs_->Derive(), this->rhs_->Derive());
}

std::unique_ptr<Node> AdditionNode::Copy() const
{
    return std::make_unique<AdditionNode>(this->lhs_->Copy(), this->rhs_->Copy());
}

llvm::Value *AdditionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFAdd(lhs, rhs, "addtmp");
}

std::vector<const Node *> BinaryNode::Children() const
{
    std::vector<Node const *> children{this};
    for(auto const &child : this->lhs_->Children())
    {
        children.push_back(child);
    }

    for(auto const &child : this->rhs_->Children())
    {
        children.push_back(child);
    }

    return children;
}

std::unique_ptr<Node> SubtractionNode::Derive() const
{
    return std::make_unique<SubtractionNode>(this->lhs_->Derive(), this->rhs_->Derive());
}

std::unique_ptr<Node> SubtractionNode::Copy() const
{
    return std::make_unique<SubtractionNode>(this->lhs_->Copy(), this->rhs_->Copy());
}

llvm::Value *SubtractionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFSub(lhs, rhs, "subtmp");
}

std::unique_ptr<Node> MultiplicationNode::Derive() const
{
    // Follow Product Rule
    // TODO: Add checks for if either rhs or lhs is constant, because then the product rule changes!
    auto first_product = std::make_unique<MultiplicationNode>(this->lhs_->Derive(), this->rhs_->Copy());
    auto second_product = std::make_unique<MultiplicationNode>(this->lhs_->Copy(), this->rhs_->Derive());

    return std::make_unique<AdditionNode>(std::move(first_product), std::move(second_product));
}

std::unique_ptr<Node> MultiplicationNode::Copy() const
{
    return std::make_unique<MultiplicationNode>(this->lhs_->Copy(), this->rhs_->Copy());
}

llvm::Value *MultiplicationNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFMul(lhs, rhs, "multmp");
}

std::unique_ptr<Node> DivisionNode::Derive() const
{
    // Follow Quotient Rule
    auto first_product = std::make_unique<MultiplicationNode>(this->lhs_->Derive(), this->rhs_->Copy());
    auto second_product = std::make_unique<MultiplicationNode>(this->lhs_->Copy(), this->rhs_->Derive());

    auto difference = std::make_unique<SubtractionNode>(std::move(first_product), std::move(second_product));

    auto g_squared = std::make_unique<MultiplicationNode>(this->rhs_->Copy(), this->rhs_->Copy());

    return std::make_unique<DivisionNode>(std::move(difference), std::move(g_squared));
}

std::unique_ptr<Node> DivisionNode::Copy() const
{
    return std::make_unique<DivisionNode>(this->lhs_->Copy(), this->rhs_->Copy());
}

llvm::Value *DivisionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFDiv(lhs, rhs, "divtmp");
}

std::unique_ptr<Node> PotentiationNode::Derive() const {
    return std::unique_ptr<Node>();
}

std::unique_ptr<Node> PotentiationNode::Copy() const {
    return std::make_unique<PotentiationNode>(this->lhs_->Copy(), this->rhs_->Copy());
}

llvm::Value *PotentiationNode::Codegen(CompilationContext &ctx) {
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    llvm::Function *pow = ctx.module->getFunction("pow");

    return ctx.builder->CreateCall(pow, {lhs, rhs}, "powtmp");
}
