#ifndef UNLOGIC_IRGENERATOR_H
#define UNLOGIC_IRGENERATOR_H

#include <stack>
#include "parser/Node.h"

namespace unlogic
{
    class Scope
    {
        std::vector<std::map<std::string, llvm::Value*>> layers;

    public:
        std::optional<llvm::Value*> Lookup(std::string const &key)
        {
            for(auto &layer : this->layers | std::ranges::views::reverse)
            {
                if(layer.contains(key)) return layer[key];
            }

            return std::nullopt;
        }

        void Insert(std::string const &key, llvm::Value *value)
        {
            this->layers.back()[key] = value;
        }

        void PushLayer()
        {
            this->layers.emplace_back();
        }

        void PopLayer()
        {
            this->layers.pop_back();
        }
    };

    struct IRGenerationContext
    {
        llvm::LLVMContext &llvm_ctx;
        llvm::Module &module;
        llvm::IRBuilder<> &builder;
        Scope &scope;
    };

    struct IRGenerator : public INodeVisitor
    {
        IRGenerationContext &ctx;
        std::stack<llvm::Value *> values;

        void Visit(const NumericLiteralNode *node) override;
        void Visit(const DivisionNode *node) override;
        void Visit(const ScopedBlockNode *node) override;
        void Visit(const VariableNode *node) override;
        void Visit(const CallNode *node) override;
        void Visit(const AdditionNode *node) override;
        void Visit(const SubtractionNode *node) override;
        void Visit(const MultiplicationNode *node) override;
        void Visit(const PotentiationNode *node) override;
        void Visit(const FunctionDefinitionNode *node) override;

        IRGenerator(IRGenerationContext &ctx) : ctx(ctx) {}
    };
}

#endif //UNLOGIC_IRGENERATOR_H
