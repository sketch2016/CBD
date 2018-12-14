#include "include/ui/mainwindow.h"
#include <QApplication>
#include <QSettings>

#include "include/manager/servicemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServiceManager *serviceManager = new ServiceManager();
    //serviceManager->exec();
    //MainWindow w;
    //w.show();

    return a.exec();
}
