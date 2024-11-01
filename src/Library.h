//
// Created by nathan on 6/18/24.
//

#ifndef UNLOGIC_LIBRARY_H
#define UNLOGIC_LIBRARY_H

#include <string>
#include <utility>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>

namespace unlogic
{
    struct LibraryFunction
    {
        llvm::orc::ExecutorSymbolDef symbol;
        llvm::Function *function;
    };

    struct Library
    {
        std::string name;
        llvm::LLVMContext &ctx;
        llvm::Module module;
        std::vector<LibraryFunction> functions;

        void AddFunction(char const *function_name, llvm::FunctionType *type, void *native_function)
        {
            llvm::Function *function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, function_name, this->module);
            llvm::orc::ExecutorSymbolDef symbol = {
                    llvm::orc::ExecutorAddr::fromPtr(native_function),
                    llvm::JITSymbolFlags::Callable,
            };

            this->functions.emplace_back({
                .symbol = std::move(symbol),
                .function = function,
            });
        }

        Library(std::string name, llvm::LLVMContext &ctx)
            : name(std::move(name)),
              ctx(ctx),
              module(name, ctx) {}


        Library() = delete;
        Library(Library const&) = delete;
    };

    class LibraryDefinition
    {
        std::string name_;
        std::function<void(Library&)> builder_;

    public:
        Library Build(llvm::LLVMContext &ctx)
        {
            Library library(this->name_, ctx);

            this->builder_(library);

            return std::move(library);
        }

        LibraryDefinition(std::string name, std::function<void(Library&)> builder) : name_(std::move(name)), builder_(std::move(builder)) {}
    };
}

#endif //UNLOGIC_LIBRARY_H
