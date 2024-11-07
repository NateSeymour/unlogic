#include "IRGenerator.h"

void unlogic::IRGenerator::Visit(const unlogic::NumericLiteralNode *node)
{
    llvm::Value *value = llvm::ConstantFP::get(this->ctx.llvm_ctx, llvm::APFloat(node->value));
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::StringLiteralNode *node)
{

}

void unlogic::IRGenerator::Visit(const unlogic::VariableNode *node)
{
   llvm::Value *value = *this->ctx.scope.Lookup(node->identifier_);
   this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::CallNode *node)
{
    llvm::Function *function = ctx.module->getFunction(node->function_name_);

    if(function->arg_size() < node->arguments_.size())
    {
        throw std::runtime_error("Aaaaaahhhhhhh");
    }

    std::vector<llvm::Value *> argument_values;
    argument_values.reserve(node->arguments_.size());
    for(auto const &argument : node->arguments_)
    {
        argument->Accept(*this);
        argument_values.push_back(this->values.top());
        this->values.pop();
    }

    llvm::Value *value = this->builder.CreateCall(function, argument_values, "calltmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::AdditionNode *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFAdd(lhs, rhs, "addtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::SubtractionNode *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFSub(lhs, rhs, "subtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::MultiplicationNode *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFMul(lhs, rhs, "multmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::DivisionNode *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFDiv(lhs, rhs, "divtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::PotentiationNode *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Function *pow = ctx.module->getFunction("pow");

    llvm::Value *value = this->builder.CreateCall(pow, {lhs, rhs}, "powtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(const unlogic::FunctionDefinitionNode *node)
{
    // Generate function information
    std::vector<llvm::Type*> argument_types(node->args_.size(), llvm::Type::getDoubleTy(ctx.llvm_ctx));
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx.llvm_ctx), argument_types, false);
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, node->name_, *ctx.module);

    unsigned idx = 0;
    for (auto &arg : function->args())
    {
        arg.setName(node->args_[idx++]);
    }

    // Generate function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, node->name_, function);
    this->builder.SetInsertPoint(block);

    ctx.scope.PushLayer();
    for(auto &arg : function->args())
    {
        ctx.scope.Insert(std::string(arg.getName()),  &arg);
    }

    node->body_->Accept(*this);
    llvm::Value* return_value = this->values.top();
    this->values.pop();

    this->builder.CreateRet(return_value);

    ctx.scope.PopLayer();

    llvm::verifyFunction(*function);

    this->values.push(function);
}

void unlogic::IRGenerator::Visit(const unlogic::ScopedBlockNode *node)
{
}
