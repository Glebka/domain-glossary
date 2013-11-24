#include "listener.h"

Listener::Listener(QObject *parent) :
    QTcpServer(parent)
{
}

void Listener::clientDisconnected(ClientWorker *worker)
{
    m_clients.remove(worker);
    log(QString("Client #")+QString::number((quint64)worker)+QString(" disconnected"));
}

void Listener::incomingConnection(qintptr handle)
{
    QThread * thread=new QThread(this);
    ServiceProvider * service=new ServiceProvider();
    service->moveToThread(thread);
    ClientWorker * worker=new ClientWorker(handle,service);
    worker->moveToThread(thread);
    service->setWorker(worker);
    connect(thread,&QThread::started,worker,&ClientWorker::run);
    connect(worker,&ClientWorker::finished,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    connect(thread,&QThread::finished,worker,&ClientWorker::deleteLater);
    connect(thread,&QThread::finished,service,&ServiceProvider::deleteLater);
    connect(worker,&ClientWorker::disconnect,this,&Listener::clientDisconnected);
    thread->start();
    Record rec;
    rec.thread=thread;
    rec.worker=worker;
    m_clients[worker]=rec;
}

void Listener::log(QString text)
{
    qDebug()<<QDateTime::currentDateTime().toString()<<" [Listener] "<<text;
}
