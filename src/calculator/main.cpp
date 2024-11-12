#include <QApplication>
#include <QtWidgets/QtWidgets>
#include "Window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ui::Window window;
    window.resize(1000, 500);
    window.show();

    return app.exec();
}