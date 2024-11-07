#include <thread>
#include <gtkmm.h>
#include "main.h"
#include "compiler/Compiler.h"
#include "ui/Window.h"

/*
 * Data structures:
 * - Worker thread that upon notification will do compilation tasks
 * - Worker thread that will take care of syntax highlighting
 * - Global storage for graphed objects and general results
 * - Dispatchers for synchronization
 */

void init_compiler_runtime()
{
    unlogic::Compiler::InitializeGlobalCompilerRuntime();

    unlogic::compiler_runtime_avail = true;
    unlogic::compiler_runtime_avail.notify_all();
}

int main(int argc, char *argv[])
{
    std::cout << "[MAIN] Initializing compiler runtime..." << std::endl;
    std::thread init_thread(init_compiler_runtime);

    std::cout << "[MAIN] Creating application..." << std::endl;
    auto app = Gtk::Application::create("global.seymour.unlogic");
    int ret = app->make_window_and_run<unlogic::Window>(argc, argv);

    init_thread.join();

    return ret;
}
