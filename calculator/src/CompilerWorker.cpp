#include "CompilerWorker.h"
#include "compiler/Compiler.h"
#include "compiler/std/RuntimeLibrary.h"
#include "compiler/std/StandardLibrary.h"
#include "util/overload.h"

using namespace ui;

void CompilerWorker::compileAndRun(std::string program_text)
{
    Q_EMIT statusUpdate(CompilationStatus::InProgress);

    std::vector<bf::Token<unlogic::ParserGrammarType>> tokens;
    auto result = compiler_->Compile(program_text, &tokens);

    Q_EMIT tokenizationComplete(tokens);

    if (!result.has_value())
    {
        Q_EMIT statusUpdate(CompilationStatus::Error);

        unlogic::Error error = std::visit(
                unlogic::overload{
                        [&](unlogic::Error const &error)
                        {
                            std::string message = std::format("Compilation Error: {}", error.message);
                            return unlogic::Error{message};
                        },
                        [&](bf::Error const &error)
                        {
                            std::string message = std::format("Parsing Error: {}", error.message);
                            return unlogic::Error{message};
                        },
                        [&](llvm::Error const &error) { return unlogic::Error{"LLVM Error!"}; },
                },
                result.error());

        Q_EMIT compilationError(std::move(error));
    }
    else
    {
        Q_EMIT statusUpdate(CompilationStatus::Successful);
        auto scene = std::make_shared<unlogic::Scene>();

        try
        {
            auto &program = *result;
            program(scene.get());
        }
        catch (std::exception &e)
        {
            Q_EMIT statusUpdate(CompilationStatus::Error);
            Q_EMIT compilationError(std::move(unlogic::Error{e.what()}));
            return;
        }

        Q_EMIT sceneReady(std::move(scene));
    }
}

CompilerWorker::CompilerWorker()
{
    std::vector<unlogic::Library *> libs = {&unlogic::stdlib, &unlogic::runtime};
    this->compiler_ = std::make_unique<unlogic::Compiler>(libs);
}

CompilerWorker::~CompilerWorker() = default;
