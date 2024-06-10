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
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>
#include "Parser.h"

namespace unlogic
{
    struct CompilerRuntime
    {
        std::unique_ptr<llvm::orc::ExecutionSession> execution_session;
        std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> object_layer;
        std::unique_ptr<llvm::orc::IRCompileLayer> compile_layer;
        llvm::orc::MangleAndInterner mangle;
        llvm::orc::JITDylib &main;

        CompilerRuntime(std::unique_ptr<llvm::orc::ExecutionSession> execution_session,
            std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> object_layer,
            std::unique_ptr<llvm::orc::IRCompileLayer> compile_layer,
            llvm::orc::JITDylib &main,
            llvm::orc::MangleAndInterner mangle) :
            execution_session(std::move(execution_session)),
            object_layer(std::move(object_layer)),
            compile_layer(std::move(compile_layer)),
            main(main),
            mangle(mangle)
        {

        }

        static std::shared_ptr<CompilerRuntime> CreateShared()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();

            auto self_exec_proc_control = llvm::orc::SelfExecutorProcessControl::Create();
            if(auto e = self_exec_proc_control.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            auto execution_session = std::make_unique<llvm::orc::ExecutionSession>(std::move(*self_exec_proc_control));

            auto object_layer = std::make_unique<llvm::orc::RTDyldObjectLinkingLayer>(*execution_session, []() { return std::make_unique<llvm::SectionMemoryManager>(); });

            llvm::orc::JITTargetMachineBuilder builder = execution_session->getExecutorProcessControl().getTargetTriple();
            auto layout = builder.getDefaultDataLayoutForTarget();
            if(auto e = layout.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            auto concurrent_compiler = std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(builder));
            auto compile_layer = std::make_unique<llvm::orc::IRCompileLayer>(*execution_session, *object_layer, std::move(concurrent_compiler));

            llvm::orc::JITDylib &main = execution_session->createBareJITDylib("<main>");

            auto generator = llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout->getGlobalPrefix()));
            main.addGenerator(std::move(generator));

            llvm::orc::MangleAndInterner mangle(*execution_session, *layout);

            if(builder.getTargetTriple().isOSBinFormatCOFF())
            {
                object_layer->setOverrideObjectFlagsWithResponsibilityFlags(true);
                object_layer->setAutoClaimResponsibilityForObjectSymbols(true);
            }

            return std::make_shared<CompilerRuntime>(std::move(execution_session), std::move(object_layer), std::move(compile_layer), main, mangle);
        }

        ~CompilerRuntime()
        {
            this->execution_session->endSession();
        }
    };

    template<typename... Args>
    class Callable
    {
        friend class Compiler;

        std::shared_ptr<CompilerRuntime> runtime_;
        double(*function)(Args...) = nullptr;

    protected:
        Callable(std::shared_ptr<CompilerRuntime> runtime, llvm::orc::ExecutorSymbolDef function) : runtime_(std::move(runtime))
        {
            this->function = function.getAddress().toPtr < double(*)(Args...) > ();
        }

    public:
        double operator()(Args... args)
        {
            return this->function(args...);
        }

        Callable() = delete;
    };

    class Compiler
    {
        std::shared_ptr<CompilerRuntime> runtime_;

    public:
        template<typename... Args>
        Callable<Args...> CompileFunction(std::string const &input)
        {
            Parser parser(input);

            auto function_definition = parser.ParseFunctionDefinition();

            CompilationContext ctx;
            auto function_raw = function_definition.Codegen(ctx);

            llvm::orc::ThreadSafeModule thread_safe_module(std::move(ctx.module), std::move(ctx.llvm_ctx));

            auto resource_tracker = this->runtime_->main.getDefaultResourceTracker();
            this->runtime_->compile_layer->add(resource_tracker, std::move(thread_safe_module));

            auto function = this->runtime_->execution_session->lookup({&this->runtime_->main}, this->runtime_->mangle(function_definition.name)).get();

            return std::move(Callable<Args...>(this->runtime_, function));
        }

        Compiler()
        {
            this->runtime_ = CompilerRuntime::CreateShared();
        }
    };
}

#endif //UNLOGIC_COMPILER_H
