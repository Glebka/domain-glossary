#include "clientworker.h"

ClientWorker::ClientWorker(qintptr socketHandle, QObject *parent) :
    QObject(parent),
    m_handle(socketHandle),
    m_socket(0)
{

}

void ClientWorker::timerEvent(QTimerEvent *evt)
{
    evt->accept();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << "Hello";
    //out.device()->seek(0);
    //out << (quint16)(block.size() - sizeof(quint16));

    m_socket->write(block);
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    emit finished();
    emit disconnect(this);
}

void ClientWorker::run()
{
    m_socket=new QTcpSocket(this);
    if(!m_socket->setSocketDescriptor(m_handle))
        qDebug()<<"[#"<<(quint64)this<<"]Error opening socket";
    else
        qDebug()<<"[#"<<(quint64)this<<"]Client connected";
    this->startTimer(5000);
}
