#include "listener.h"

Listener::Listener(QObject *parent) :
    QTcpServer(parent)
{
}

void Listener::clientDisconnected(ClientWorker *worker)
{
    m_clients.remove(worker);
    qDebug()<<"Client #"<<(quint64)worker<<" was disconnected.";
}

void Listener::incomingConnection(qintptr handle)
{
    QThread * thread=new QThread(this);
    ClientWorker * worker=new ClientWorker(handle);
    worker->moveToThread(thread);
    connect(thread,&QThread::started,worker,&ClientWorker::run);
    connect(worker,&ClientWorker::finished,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    connect(thread,&QThread::finished,worker,&ClientWorker::deleteLater);
    connect(worker,&ClientWorker::disconnect,this,&Listener::clientDisconnected);
    thread->start();
    Record rec;
    rec.thread=thread;
    rec.worker=worker;
    m_clients[worker]=rec;
}
