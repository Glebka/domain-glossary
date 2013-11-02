#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <listener.h>
#include <dataprovider.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //DataProvider provider;
    //QThread data_provider_thread;
    //provider.moveToThread(&data_provider_thread);

    //QCoreApplication::connect(&data_provider_thread,&QThread::started,&provider,&DataProvider::run);
    //data_provider_thread.start();

    //provider.initFromXml();
    //provider.initFromBinary();
    DataProvider::getInstance();
    Listener server;
    if(!server.listen(QHostAddress::AnyIPv4,1111))
    {
        qDebug()<<"Error!";
    }
    //provider.configToXML();
    //provider.contentsToXML();
    //provider.contentsToBin();
    return a.exec();
}
