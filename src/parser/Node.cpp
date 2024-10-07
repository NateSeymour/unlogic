#include "Node.h"

using namespace unlogic;

llvm::Value *VariableNode::Codegen(CompilationContext &ctx)
{
    return ctx.named_values[this->identifier_];
}

llvm::Value *NumericLiteralNode::Codegen(CompilationContext &ctx)
{
    return llvm::ConstantFP::get(*ctx.llvm_ctx, llvm::APFloat(this->value_));
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

llvm::Value *AdditionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFAdd(lhs, rhs, "addtmp");
}

llvm::Value *SubtractionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFSub(lhs, rhs, "subtmp");
}

llvm::Value *MultiplicationNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFMul(lhs, rhs, "multmp");
}

llvm::Value *DivisionNode::Codegen(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    return ctx.builder->CreateFDiv(lhs, rhs, "divtmp");
}

llvm::Value *PotentiationNode::Codegen(CompilationContext &ctx) {
    llvm::Value *lhs = this->lhs_->Codegen(ctx);
    llvm::Value *rhs = this->rhs_->Codegen(ctx);

    llvm::Function *pow = ctx.module->getFunction("pow");

    return ctx.builder->CreateCall(pow, {lhs, rhs}, "powtmp");
}
