#include <QCoreApplication>
#include <listener.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Listener server;
    if(!server.listen(QHostAddress::AnyIPv4,1111))
    {
        qDebug()<<"Error!";
    }
    return a.exec();
}
