#include <QCoreApplication>
#include <QDebug>

#include <listener.h>
#include <dataprovider.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DataProvider provider;
    provider.init();

    Listener server;
    if(!server.listen(QHostAddress::AnyIPv4,1111))
    {
        qDebug()<<"Error!";
    }
    provider.configToXML();
    provider.contentsToXML();
    return a.exec();
}
