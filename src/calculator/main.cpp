#include <gtkmm.h>
#include "Compiler.h"
#include "ui/Window.h"

int main(int argc, char *argv[])
{
    unlogic::Compiler::InitializeCompilerRuntime();

    auto app = Gtk::Application::create("global.seymour.unlogic");
    return app->make_window_and_run<unlogic::Window>(argc, argv);
}
