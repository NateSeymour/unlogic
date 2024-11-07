#ifndef UNLOGIC_PROGRAM_H
#define UNLOGIC_PROGRAM_H

#include "transformer/IRGenerator.h"
#include "parser/Node.h"

namespace unlogic
{
    class Program
    {
        llvm::LLVMContext &llvm_ctx_;
        llvm::IRBuilder<> builder_;

        std::unique_ptr<Node> body_;

    public:
        llvm::Module Build()
        {
            llvm::Module module("unlogic", this->llvm_ctx_);

            // Create IR generation context
            Scope program_scope;
            IRGenerationContext ctx = {
                    .llvm_ctx = this->llvm_ctx_,
                    .module = module,
                    .builder = this->builder_,
                    .scope = program_scope,
            };

            // IR Generator
            IRGenerator generator(ctx);

            // Generate main wrapper function
            auto main = std::make_unique<FunctionDefinitionNode>("main", std::move(this->body_));

            // Build program
            generator.Visit(main.get());

            return std::move(module);
        }

        Program() = delete;

        Program(llvm::LLVMContext &llvm_ctx, std::unique_ptr<Node> body)
            : body_(std::move(body)),
              llvm_ctx_(llvm_ctx),
              builder_(llvm_ctx) {}
    };
}

#endif //UNLOGIC_PROGRAM_H
