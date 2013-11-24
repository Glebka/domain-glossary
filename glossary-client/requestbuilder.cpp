#include "requestbuilder.h"

RequestBuilder::RequestBuilder(QObject *parent) :
    QObject(parent)
  , m_proxy(ClientSideProxy::getInstance())
{

}

QAbstractSocket::SocketState RequestBuilder::state()
{
    if(m_proxy)
        return m_proxy->state();
    else
        return QTcpSocket::UnconnectedState;
}

bool RequestBuilder::tryConnect()
{
    if(m_proxy)
        return m_proxy->tryConnect();
    else
        return false;
}

void RequestBuilder::login(QString user, QString password)
{
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<user<<password;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_LOGIN),Q_ARG(QByteArray,buffer.buffer()));
    //m_proxy->sendData(CMD_LOGIN,buffer.buffer());
}

void RequestBuilder::getAllDomains()
{
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_ALL_DOMAINS),Q_ARG(QByteArray,QByteArray()));
}

void RequestBuilder::getAllTermsByDomain(quint32 domainId)
{
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<domainId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_TERMS_BY_DOMAIN),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getConcept(quint32 conceptId)
{
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<conceptId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_CONCEPT),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::on_getAllDomains(PacketHeader header, QBufferPtr data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllDomains";
    QDataStream stream(data);
    QList<DomainInfo> list;
    stream>>list;
    emit loadDomains(list);
}

void RequestBuilder::on_getAllTerms(PacketHeader header, QBufferPtr data)
{
}

void RequestBuilder::on_getConcept(PacketHeader header, QBufferPtr data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllDomains";
    QDataStream stream(data);
    ConceptInfo ci;
    stream>>ci;
    emit loadConcept(ci);
}

void RequestBuilder::on_getConceptText(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_getDomainById(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_getTerm(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_getTermsByDomain(PacketHeader header, QBufferPtr data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllTermsByDomain";
    QDataStream stream(data);
    QList<TermInfo> list;
    stream>>list;
    emit loadTermsByDomain(list);
}

void RequestBuilder::on_getUserById(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_search(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_login(PacketHeader header, QBufferPtr data)
{
    if(header.status!=CMD_OK_NOCACHE)
    {
        qDebug()<<"Login incorrect!";
        emit loggedIn(false);
    }
    else
    {
        QDataStream stream(data);
        UserInfo user;
        stream>>user;
        emit loggedIn(true);
    }
}

void RequestBuilder::on_error(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_forbidden(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_notFound(PacketHeader header, QBufferPtr data)
{

}

void RequestBuilder::on_timeout(PacketHeader header, QBufferPtr data)
{

}
