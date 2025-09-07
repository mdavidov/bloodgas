#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Blood Gas Analyzer");
    app.setOrganizationName("mdavidov");
    app.setApplicationVersion("1.0.0");
    
    MainWindow wnd;
    wnd.show();

    return app.exec();
}
