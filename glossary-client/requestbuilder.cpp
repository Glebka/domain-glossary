#include "requestbuilder.h"

RequestBuilder::RequestBuilder(QObject *parent) :
    QObject(parent)
  , m_proxy(ClientSideProxy::getInstance())
  , m_is_transaction(false)
{
    m_methods[CMD_GET_ALL_DOMAINS]="on_getAllDomains";
    m_methods[CMD_GET_ALL_TERMS]="on_getAllTerms";
    m_methods[CMD_GET_CONCEPT]="on_getConcept";
    m_methods[CMD_GET_CONCEPT_TEXT]="on_getConceptText";
    m_methods[CMD_GET_DOMAIN]="on_getDomainById";
    m_methods[CMD_GET_TERM]="on_getTerm";
    m_methods[CMD_GET_TERMS_BY_DOMAIN]="on_getTermsByDomain";
    m_methods[CMD_GET_USER]="on_getUserById";
    m_methods[CMD_SEARCH]="on_search";

    m_methods[CMD_ADD_TERM]="on_addTerm";
    m_methods[CMD_ADD_TERM_TO_EXISTING]="on_addTerm";

    m_methods[CMD_LOGIN]="on_login";
    m_methods[CMD_ERROR]="on_error";
    m_methods[CMD_FORBIDDEN]="on_forbidden";
    m_methods[CMD_NOT_FOUND]="on_notFound";
    m_methods[CMD_TIMEOUT]="on_timeout";
    m_transaction.open(QIODevice::ReadWrite);
    qRegisterMetaType<QList<TermInfo>>("QList<TermInfo>");
    qRegisterMetaType<QList<DomainInfo>>("QList<DomainInfo>");
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

void RequestBuilder::disconnectFromHost()
{
    user=UserInfo();
    m_queue.clear();
    m_transaction.buffer().clear();
    m_is_transaction=false;
    m_transactionClosed.wakeAll();
    m_proxy->disconnectFromServer();
}

void RequestBuilder::startTransaction()
{
    //QMutexLocker locker(&m_start_mutex);
    //if(m_is_transaction||!m_queue.isEmpty())
    //    m_activeTransaction.wait(&m_start_mutex);
    m_is_transaction=true;
    m_queue.clear();
    m_transaction.seek(0);
    m_transaction.buffer().clear();
    log("Transaction started...");
}

QByteArray RequestBuilder::endTransaction()
{
    log("Waiting to close transaction...");
    QMutexLocker locker(&m_mutex);
    m_transactionClosed.wait(&m_mutex);
    QBuffer buffer;
    buffer.setData(m_transaction.buffer());
    log("Transaction closed");
    //m_activeTransaction.wakeAll();
    return buffer.data();
}

void RequestBuilder::login(QString user, QString password)
{
    log("login");
    if(m_is_transaction)
        m_queue.enqueue(CMD_LOGIN);
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
    log("getAllDomains");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_ALL_DOMAINS);
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_ALL_DOMAINS),Q_ARG(QByteArray,QByteArray()));
}

void RequestBuilder::getDomainById(quint32 domainId)
{
    log("getDomainsById");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_DOMAIN);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<domainId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_DOMAIN),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getAllTermsByDomain(quint32 domainId)
{
    log("getAllTermsByDomain");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_TERMS_BY_DOMAIN);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<domainId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_TERMS_BY_DOMAIN),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getConcept(quint32 conceptId)
{
    log("getConcept");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_CONCEPT);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<conceptId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_CONCEPT),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getConceptText(quint32 conceptId)
{
    log("getConceptText");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_CONCEPT_TEXT);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<conceptId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_CONCEPT_TEXT),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getAllTerms(quint32 start, quint32 length)
{
    log("getAllTerms "+QString::number(start)+","+QString::number(length));
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_ALL_TERMS);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<start<<length;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_ALL_TERMS),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::getTerm(quint32 termId)
{
    log("getTerm");
    if(m_is_transaction)
        m_queue.enqueue(CMD_GET_TERM);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<termId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_GET_TERM),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::search(QString search)
{
    log("search");
    if(m_is_transaction)
        m_queue.enqueue(CMD_SEARCH);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<search;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_SEARCH),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::addTerm(QString term)
{
    log("addTerm");
    if(user.type!=Expert)
    {
        QMessageBox::warning(0,"Добавление термина","Операция не разрешена. Войдите в систему, используя учетную запись эксперта.");
        return;
    }
    if(m_is_transaction)
        m_queue.enqueue(CMD_ADD_TERM);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<term;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_ADD_TERM),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::addTermToExisting(QString term, quint32 anotherTermId)
{
    log("addTermToExisting");
    if(user.type!=Expert)
    {
        QMessageBox::warning(0,"Добавление термина","Операция не разрешена. Войдите в систему, используя учетную запись эксперта.");
        return;
    }
    if(m_is_transaction)
        m_queue.enqueue(CMD_ADD_TERM_TO_EXISTING);
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    QDataStream stream(&buffer);
    stream<<term<<anotherTermId;
    QMetaObject::invokeMethod(m_proxy,"sendData",Qt::QueuedConnection,Q_ARG(quint32,CMD_ADD_TERM_TO_EXISTING),Q_ARG(QByteArray,buffer.buffer()));
}

void RequestBuilder::on_responseReady(PacketHeader header, QBufferPtr data)
{
    log("Queue size:"+QString::number(m_queue.size()));
    /*if(header.command!=m_queue.head()||(header.status!=CMD_OK && header.status!=CMD_OK_NOCACHE))
    {
        if(m_is_transaction)
        {
            terminateTransaction();
            return;
        }
    }
    m_queue.dequeue();
    if(m_is_transaction)
    {
        m_transaction.write(data->buffer());
        if(m_queue.isEmpty())
        {
            m_is_transaction=false;
            m_transaction.seek(0);
            m_transactionClosed.wakeAll();
        }
        return;
    }*/
    if(m_is_transaction)
    {
        qDebug()<<"Ok, this response will be appended to transaction";
        if(header.command!=m_queue.head()||(header.status!=CMD_OK && header.status!=CMD_OK_NOCACHE))
        {
            qDebug()<<"Terminating transaction";
            terminateTransaction();
            return;
        }
        qDebug()<<"Deque";
        m_queue.dequeue();
        qDebug()<<"Queue size "<<m_queue.size();
        m_transaction.write(data->buffer());
        if(m_queue.isEmpty())
        {
            qDebug()<<"Queue empty, close transaction";
            m_is_transaction=false;
            m_transaction.seek(0);
            m_transactionClosed.wakeAll();
            m_activeTransaction.wakeAll();
        }
        return;
    }
    if(m_methods.contains(header.command))
    {
        QString method=m_methods[header.command];
        log("QMetaObject::invokeMethod "+method);
        QMetaObject::invokeMethod(this,method.toStdString().c_str(),Qt::QueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QByteArray,data->buffer()));
    }
    else
    {
        log("Unknown command");
        header.data_length=0;
        header.command=CMD_ERROR;
        header.status=CMD_ERROR;
        data->seek(0);
        data->buffer().clear();
        QMetaObject::invokeMethod(this,m_methods[CMD_ERROR].toStdString().c_str(),Qt::QueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QByteArray,data->buffer()));
    }
    if(m_queue.isEmpty())
        m_activeTransaction.wakeAll();
}

void RequestBuilder::on_getAllDomains(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllDomains";
    QDataStream stream(&data,QIODevice::ReadOnly);
    QList<DomainInfo> list;
    stream>>list;
    emit loadDomains(list);
}

void RequestBuilder::on_getAllTerms(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllTerms";
    QDataStream stream(&data,QIODevice::ReadOnly);
    QList<TermInfo> list;
    stream>>list;
    emit loadTerms(list);
}

void RequestBuilder::on_getConcept(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllDomains";
    QDataStream stream(&data,QIODevice::ReadOnly);
    ConceptInfo ci;
    stream>>ci;
    emit loadConcept(ci);
}

void RequestBuilder::on_getConceptText(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getConceptText";
    QDataStream stream(&data,QIODevice::ReadOnly);
    QString text;
    stream>>text;
    emit loadConceptText(text);
}

void RequestBuilder::on_getDomainById(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getDomainById";
    QDataStream stream(&data,QIODevice::ReadOnly);
    DomainInfo di;
    stream>>di;
    emit loadDomainById(di);
}

void RequestBuilder::on_getTerm(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getTerm";
    QDataStream stream(&data,QIODevice::ReadOnly);
    TermInfo ti;
    stream>>ti;
    emit loadTerm(ti);
}

void RequestBuilder::on_getTermsByDomain(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on getAllTermsByDomain";
    QDataStream stream(&data,QIODevice::ReadOnly);
    QList<TermInfo> list;
    stream>>list;
    emit loadTermsByDomain(list);
}

void RequestBuilder::on_getUserById(PacketHeader header, QByteArray data)
{

}

void RequestBuilder::on_search(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK)
        qDebug()<<"Error on search";
    QDataStream stream(&data,QIODevice::ReadOnly);
    QList<quint32> list;
    stream>>list;
    emit loadSearch(list);
}

void RequestBuilder::on_addTerm(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK_NOCACHE)
        qDebug()<<"Error on AddTerm";
    QDataStream stream(&data,QIODevice::ReadOnly);
    TermInfo ti;
    stream>>ti;
    emit termAdded(ti);
}

void RequestBuilder::on_login(PacketHeader header, QByteArray data)
{
    if(header.status!=CMD_OK_NOCACHE)
    {
        qDebug()<<"Login incorrect!";
        emit loggedIn(false);
    }
    else
    {
        QDataStream stream(&data,QIODevice::ReadOnly);
        UserInfo user;
        stream>>user;
        this->user=user;
        emit loggedIn(true);
    }
}

void RequestBuilder::on_error(PacketHeader header, QByteArray data)
{

}

void RequestBuilder::on_forbidden(PacketHeader header, QByteArray data)
{

}

void RequestBuilder::on_notFound(PacketHeader header, QByteArray data)
{

}

void RequestBuilder::on_timeout(PacketHeader header, QByteArray data)
{

}

void RequestBuilder::terminateTransaction()
{
    m_is_transaction=false;
    m_transaction.seek(0);
    m_transaction.buffer().clear();
    m_queue.clear();
    m_transactionClosed.wakeAll();
    m_activeTransaction.wakeAll();
}

void RequestBuilder::log(QString text)
{
    qDebug()<<"[RequestBuilder]"<<text;
}
