#include "clientworker.h"

ClientWorker::ClientWorker(qintptr socketHandle,ServiceProvider * service, QObject *parent)
    : QObject(parent)
    , m_handle(socketHandle)
    , m_socket(0)
    , m_service(service)
    , m_packet_started(false)
{
    qRegisterMetaType<PacketHeader>("PacketHeader");
    qRegisterMetaType<QBufferPtr>("QBufferPtr");

    m_methods[CMD_GET_ALL_DOMAINS]="getAllDomains";
    m_methods[CMD_GET_ALL_TERMS]="getAllTerms";
    m_methods[CMD_GET_CONCEPT]="getConcept";
    m_methods[CMD_GET_CONCEPT_TEXT]="getConceptText";
    m_methods[CMD_GET_DOMAIN]="getDomainById";
    m_methods[CMD_GET_TERM]="getTerm";
    m_methods[CMD_GET_TERMS_BY_DOMAIN]="getTermsByDomain";
    m_methods[CMD_GET_USER]="getUserById";
    m_methods[CMD_SEARCH]="search";

    m_methods[CMD_ADD_TERM]="addTerm";
    m_methods[CMD_ADD_TERM_TO_EXISTING]="addTermToExisting";
    m_methods[CMD_EDIT_TERM]="editTerm";

    m_methods[CMD_LOGIN]="login";
    connect(&m_timer,&QTimer::timeout,this,&ClientWorker::timeout);
}

ClientWorker::~ClientWorker()
{
}

void ClientWorker::run()
{
    m_socket=new QTcpSocket(this);

    if(!m_socket->setSocketDescriptor(m_handle))
        log("Error opening socket");
    else
    {
        connect(m_socket, &QTcpSocket::readyRead, this, &ClientWorker::onReadyRead);
        connect(m_socket,&QTcpSocket::disconnected,this, &ClientWorker::finished);
        connect(m_socket,&QTcpSocket::disconnected,this, &ClientWorker::onDisconnected);
        //m_socket->setReadBufferSize(SOCKET_BUFFER_SIZE);
        m_socket_data_stream.setDevice(m_socket);
        Q_ASSERT(m_raw_buffer.open(QIODevice::ReadWrite));
        log("Client connected");
        //qDebug()<<"[#"<<(quint64)this<<"]Client connected";
        Q_ASSERT(m_service);
    }
}
qint64 ClientWorker::readData()
{
    log("readData()");
    QByteArray bytes=m_socket->read(m_header.data_length);
    m_readed+=bytes.size();
    m_portions++;
    qint64 written=m_raw_buffer.write(bytes);
    m_header.data_length-=written;
    if(m_header.data_length<0)
        log("More data, than needed.");
    if(m_header.data_length<=0)
    {
        m_packet_started=false;
        m_raw_buffer.seek(0);
        m_timer.stop();
        qDebug()<<m_readed<<" bytes readed in "<<m_portions<<" portions";
        dataArrived();
        if(m_socket->bytesAvailable())
        {
            qDebug()<<"More data available... Read it.";
            onReadyRead();
        }
    }
    return bytes.size();
}

void ClientWorker::sendResponse(QByteArray &data)
{
    qint64 written=0;
    qint64 portions=0;
    m_header.data_length=data.size();
    m_socket_data_stream<<m_header;
    if(m_header.status==CMD_OK || m_header.status==CMD_OK_NOCACHE)
    {
        QBuffer buffer(&data);
        Q_ASSERT(buffer.open(QIODevice::ReadOnly));
        while(!buffer.atEnd())
        {
            log("sendResponse()");
            written+=m_socket->write(buffer.read(buffer.size()));
            m_socket->waitForBytesWritten();
            portions++;
        }
        buffer.close();
    }
    qDebug()<<written<<" bytes written in "<<portions<<" portions of "<<data.size();
}

void ClientWorker::sendResponse()
{
    log("sendResponse(data_length=0)");
    m_header.data_length=0;
    m_socket_data_stream<<m_header;
}

void ClientWorker::onReadyRead()
{
    qint64 data_length=m_socket->bytesAvailable();
    if(m_packet_started)
    {
        readData();
    }
    else
    {
        if(data_length<HEADER_LENGTH)
        {
            m_socket->readAll();
            m_header={PACKET_START,CMD_ERROR,CMD_ERROR,0};
            m_socket_data_stream<<m_header;
            log("Available data size less then HEADER_SIZE. Ignoring");
            return;
        }
        m_socket_data_stream>>m_header;
        if(m_header.start_flag!=PACKET_START)
        {
            log("Bad header. Ignoring");
            if(!m_socket->atEnd())
                m_socket->readAll();
            return;
        }
        m_packet_started=true;
        m_portions=0;
        m_readed=0;
        m_raw_buffer.seek(0);
        m_raw_buffer.buffer().clear();
        if(!m_timer.isActive())
        {
            m_timer.setSingleShot(true);
            m_timer.start(10000);
        }
        readData();
    }
}

void ClientWorker::dataArrived()
{
    log("dataArrived()");
    if(m_methods.contains(m_header.command))
    {
        QString method=m_methods[m_header.command];
        log("dataArrived() invokeMethod "+method);
        QMetaObject::invokeMethod(m_service,method.toStdString().c_str(),Qt::DirectConnection,Q_ARG(PacketHeader,m_header),Q_ARG(QBufferPtr,&m_raw_buffer));
    }
    else
    {
        log("Unknown command");
        m_header.data_length=0;
        m_header.command=CMD_ERROR;
        m_header.status=CMD_ERROR;
        m_raw_buffer.seek(0);
        m_raw_buffer.buffer().clear();
        sendResponse();
    }
}

void ClientWorker::resultReady(PacketHeader header, QByteArray result)
{
    log("resultReady()");
    m_raw_buffer.seek(0);
    m_raw_buffer.buffer().clear();
    m_header=header;
    sendResponse(result);
}

void ClientWorker::timeout()
{
    m_socket->readAll();
    m_raw_buffer.seek(0);
    m_raw_buffer.buffer().clear();
    m_packet_started=false;
    log("Timeout");
}

void ClientWorker::onDisconnected()
{
    emit disconnect(this);
}

void ClientWorker::log(QString text)
{
    qDebug()<<"[#"<<(quint64)this<<"] "<<text;
}
