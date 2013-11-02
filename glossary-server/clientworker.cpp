#include "clientworker.h"

ClientWorker::ClientWorker(qintptr socketHandle, QObject *parent)
    : QObject(parent)
    , m_handle(socketHandle)
    , m_socket(0)
    , m_provider(DataProvider::getInstance())
{
}

void ClientWorker::run()
{
    m_socket=new QTcpSocket(this);
    if(!m_socket->setSocketDescriptor(m_handle))
        qDebug()<<"[#"<<(quint64)this<<"]Error opening socket";
    else
    {
        connect(m_socket, &QTcpSocket::readyRead, this, &ClientWorker::dataArrived);
        m_stream.setDevice(m_socket);
        qDebug()<<"[#"<<(quint64)this<<"]Client connected";
        Q_ASSERT(m_provider);
        connect(this,&ClientWorker::loginRequest,m_provider,&DataProvider::login);
    }
}

void ClientWorker::dataArrived()
{
    Q_ASSERT(m_provider);

    quint32 header=0;
    m_stream>>header;

    switch(header)
    {
    case CMD_LOGIN:
    {
        qDebug()<<"[#"<<(quint64)this<<"]Login request";
        QString user;
        QString password;
        m_stream>>user;
        m_stream>>password;
        emit loginRequest(user,password);
    }
        break;
    default:
        break;
    }
}

void ClientWorker::onLogin(UserInfo ui, bool success)
{
    if(success)
    {
        m_stream<<CMD_OK;
        m_stream<<ui;
        qDebug()<<ui.full_name;
    }
    else
        m_stream<<CMD_FORBIDDEN;
}
