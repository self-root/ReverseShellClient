#include <QCoreApplication>

#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Client *c = new Client;
    c->connectToServer();

    return a.exec();
}
