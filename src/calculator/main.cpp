#include <QApplication>
#include <QtWidgets/QtWidgets>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(100, 100);
    window.show();

    return app.exec();
}