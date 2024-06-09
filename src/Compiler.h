//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <string>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include "Parser.h"

namespace unlogic
{
    class Callable
    {
        llvm::orc::ExecutionSession execution_session;
        llvm::orc::RTDyldObjectLinkingLayer object_layer;
        llvm::orc::IRCompileLayer compile_layer;

        Parser parser;
    public:
        Callable(std::string const &unlogic_function) :
            parser(unlogic_function),
            execution_session(std::move(*llvm::orc::SelfExecutorProcessControl::Create())),
            object_layer(this->execution_session, []() { return std::make_unique<llvm::SectionMemoryManager>(); }),
            compile_layer(this->execution_session, )
        {
            auto prototype = this->parser.ParseFunctionDefinition();
        }

        ~Callable()
        {
            this->execution_session.endSession();
        }
    };
}

#endif //UNLOGIC_COMPILER_H
