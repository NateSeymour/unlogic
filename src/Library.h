//
// Created by nathan on 6/18/24.
//

#ifndef UNLOGIC_LIBRARY_H
#define UNLOGIC_LIBRARY_H

#include <string>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include "parser/Node.h"

namespace unlogic
{
    class Library;

    template<typename... Args>
    struct ArgHelper
    {
        static constexpr std::size_t nargs = sizeof...(Args);
        static void* Cast(double(*function)(Args...))
        {
            return (void*)function;
        }

        consteval ArgHelper() = default;
    };

    struct LibraryFunction
    {
        void *function;
        std::uint8_t nargs;

        LibraryFunction(Library &library, std::string name, void *function, std::uint8_t nargs);
    };

    class Library
    {
        friend class Compiler;
        friend class LibraryFunction;

        std::string name_;
        std::map<std::string, LibraryFunction const*> definitions_;

    protected:
        void AddDefinition(std::string const &name, LibraryFunction const *function)
        {
            this->definitions_[name] = function;
        }

    public:
        void PopulateCompilationContext(CompilationContext &ctx) const
        {
            for(auto const &[name, definition] : this->definitions_)
            {
                ctx.RegisterLibraryFunction(name, definition->nargs);
            }
        }

        llvm::orc::SymbolMap SymbolMap(llvm::orc::LLJIT const &jit) const
        {
            llvm::orc::SymbolMap sym;

            for(auto const &[name, definition] : this->definitions_)
            {
                sym.insert({
                    jit.mangleAndIntern(name),
                    {
                        llvm::orc::ExecutorAddr::fromPtr(definition->function),
                        llvm::JITSymbolFlags::Callable
                        }
                });
            }

            return sym;
        }

        explicit Library(std::string name) : name_(std::move(name)) {}
    };

#define UNLOGIC_ARGS(...) ArgHelper<__VA_ARGS__>
#define UNLOGIC_DEFINE_LIBFUNCTION(library, name, function, args) \
    namespace {                                                    \
        static LibraryFunction name(library, #name, args::Cast(function), args::nargs);\
    }
}

#endif //UNLOGIC_LIBRARY_H
