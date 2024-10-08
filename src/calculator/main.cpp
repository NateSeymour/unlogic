#include <gtkmm.h>
#include "Compiler.h"
#include "std/StandardLibrary.h"
#include "ui/Window.h"

int main(int argc, char *argv[])
{
    unlogic::Compiler::InitializeCompilerRuntime();
    unlogic::Compiler::RegisterGlobalLibrary(unlogic::stdlib);

    auto app = Gtk::Application::create("global.seymour.unlogic");
    return app->make_window_and_run<unlogic::Window>(argc, argv);
}
