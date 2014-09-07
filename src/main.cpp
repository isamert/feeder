#include "maintray.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("indicator-rss");
    a.setApplicationVersion("0.1");
    a.setOrganizationName("isamert");
    a.setOrganizationDomain("http://github.com/isamert");

    MainTray w;
    w.show();

    return a.exec();
}
