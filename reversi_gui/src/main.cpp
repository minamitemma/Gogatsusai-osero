#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("Reversi Game - 6x6");
    window.resize(800, 700);
    window.show();
    
    return app.exec();
}
