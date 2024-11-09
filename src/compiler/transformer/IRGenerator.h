#ifndef UNLOGIC_IRGENERATOR_H
#define UNLOGIC_IRGENERATOR_H

#include <stack>
#include "parser/Node.h"
#include "IRGenerationContext.h"

namespace unlogic
{
    struct IRGenerator : public INodeVisitor
    {
        IRGenerationContext &ctx;
        llvm::IRBuilder<> builder;
        std::stack<llvm::Value *> values;

        void Visit(const NumericLiteralNode *node) override;
        void Visit(const StringLiteralNode *node) override;
        void Visit(const DivisionNode *node) override;
        void Visit(const ScopedBlockNode *node) override;
        void Visit(const VariableNode *node) override;
        void Visit(const CallNode *node) override;
        void Visit(const AdditionNode *node) override;
        void Visit(const SubtractionNode *node) override;
        void Visit(const MultiplicationNode *node) override;
        void Visit(const PotentiationNode *node) override;
        void Visit(const FunctionDefinitionNode *node) override;
        void Visit(const ProgramEntryNode *node) override;

        IRGenerator(IRGenerationContext &ctx) : ctx(ctx), builder(ctx.llvm_ctx) {}
    };
}

#endif //UNLOGIC_IRGENERATOR_H
