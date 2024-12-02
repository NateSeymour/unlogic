#ifndef UNLOGIC_IRGENERATOR_H
#define UNLOGIC_IRGENERATOR_H

#include <llvm/IR/IRBuilder.h>
#include <stack>
#include "IRGenerationContext.h"
#include "parser/Node.h"

namespace unlogic
{
    struct IRGenerator
    {
        IRGenerationContext &ctx;
        llvm::IRBuilder<> builder;
        std::stack<llvm::Value *> values;

        llvm::Value *operator()(std::monostate &node);
        llvm::Value *operator()(NumericLiteralNode &node);
        llvm::Value *operator()(StringLiteralNode &node);
        llvm::Value *operator()(DivisionNode &node);
        llvm::Value *operator()(ScopedBlockNode &node);
        llvm::Value *operator()(VariableNode &node);
        llvm::Value *operator()(CallNode &node);
        llvm::Value *operator()(AdditionNode &node);
        llvm::Value *operator()(SubtractionNode &node);
        llvm::Value *operator()(MultiplicationNode &node);
        llvm::Value *operator()(PotentiationNode &node);
        llvm::Value *operator()(FunctionDefinitionNode &node);
        llvm::Value *operator()(PlotCommandNode &node);
        llvm::Value *operator()(ProgramEntryNode &node);

        IRGenerator(IRGenerationContext &ctx) : ctx(ctx), builder(ctx.llvm_ctx) {}
    };
} // namespace unlogic

#endif // UNLOGIC_IRGENERATOR_H
