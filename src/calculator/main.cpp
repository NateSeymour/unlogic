#include <gtkmm.h>
#include "Compiler.h"
#include "std/StandardLibrary.h"
#include "ui/Window.h"

/*
 * Data structures:
 * - Worker thread that upon notification will do compilation tasks
 * - Worker thread that will take care of syntax highlighting
 * - Global storage for graphed objects and general results
 * - Dispatchers for synchronization
 */

int main(int argc, char *argv[])
{
    unlogic::Compiler::InitializeGlobalCompilerRuntime();
    unlogic::Compiler::RegisterGlobalLibrary(unlogic::stdlib);

    auto app = Gtk::Application::create("global.seymour.unlogic");
    return app->make_window_and_run<unlogic::Window>(argc, argv);
}
