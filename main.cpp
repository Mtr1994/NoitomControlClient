#include <QCoreApplication>
#include "mainserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MainServer server;
    server.start();

    return a.exec();
}
