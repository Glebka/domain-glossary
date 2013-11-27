#include "clientsideproxy.h"

QThread * ClientSideProxy::m_thread=0;
ClientSideProxy * ClientSideProxy::m_instance=0;

ClientSideProxy::ClientSideProxy() :
    QObject(0)
  , m_socket(0)
  , m_config_file(0)
  , m_packet_started(false)
  , m_request_builder(0)
{
    qRegisterMetaType<PacketHeader>("PacketHeader");
    qRegisterMetaType<QBufferPtr>("QBufferPtr");

    connect(&m_timer,&QTimer::timeout,this,&ClientSideProxy::timeout);

}


void ClientSideProxy::log(QString text)
{
    qDebug()<<"[ClientSideProxy]"<<text;
}
// Add queue and timeouts!
quint64 ClientSideProxy::readData()
{
    log("readData()");
    QByteArray bytes=m_socket->read(m_header.data_length);
    m_readed+=bytes.size();
    m_portions++;
    quint64 written=m_raw_buffer.write(bytes);
    m_header.data_length-=written;
    if(m_header.data_length<0)
        log("More data, than needed.");
    if(m_header.data_length<=0)
    {
        m_packet_started=false;
        m_raw_buffer.seek(0);
        qDebug()<<m_readed<<" bytes readed in "<<m_portions<<" portions";
        dataArrived();
        if(m_socket->bytesAvailable())
            onReadyRead();
    }
}

void ClientSideProxy::dataArrived()
{
    log("dataArrived()");
    if(m_queue.head().uid!=m_header.uid)
    {
        log("Queued!=received packet");
        m_packet_started=false;
        m_raw_buffer.seek(0);
        m_raw_buffer.buffer().clear();
        return;
    }
    m_timer.stop();
    m_queue.dequeue();
    QMetaObject::invokeMethod(m_request_builder,CALLBACK,Qt::BlockingQueuedConnection,Q_ARG(PacketHeader,m_header),Q_ARG(QBufferPtr,&m_raw_buffer));
    /*if(m_methods.contains(m_header.command))
    {
        QString method=m_methods[m_header.command];
        log("QMetaObject::invokeMethod "CALLBACK);
        QMetaObject::invokeMethod(m_request_builder,CALLBACK,Qt::BlockingQueuedConnection,Q_ARG(PacketHeader,m_header),Q_ARG(QBufferPtr,&m_raw_buffer));
    }
    else
    {
        log("Unknown command");
        m_header.data_length=0;
        m_header.command=CMD_ERROR;
        m_header.status=CMD_ERROR;
        m_raw_buffer.seek(0);
        m_raw_buffer.buffer().clear();
        QMetaObject::invokeMethod(m_request_builder,m_methods[CMD_ERROR].toStdString().c_str(),Qt::BlockingQueuedConnection,Q_ARG(PacketHeader,m_header),Q_ARG(QBufferPtr,&m_raw_buffer));
    }*/
}

void ClientSideProxy::onReadyRead()
{
    qint64 data_length=m_socket->bytesAvailable();
    if(m_packet_started)
        readData();
    else
    {
        if(data_length<HEADER_LENGTH)
        {
            m_socket->readAll();
            m_header={PACKET_START,CMD_ERROR,CMD_ERROR,0};
            //m_socket_data_stream<<m_header;
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
        readData();
    }
}

ClientSideProxy *ClientSideProxy::getInstance()
{
    if(!m_instance)
    {
        m_instance=new ClientSideProxy();
        m_thread=new QThread();
        m_instance->moveToThread(m_thread);
        connect(m_thread,&QThread::started,m_instance,&ClientSideProxy::run);
        connect(m_instance,&ClientSideProxy::terminate,m_thread,&QThread::quit);
        connect(m_thread,&QThread::finished,m_thread,&QThread::deleteLater);
        connect(m_thread,&QThread::finished,m_instance,&ClientSideProxy::deleteLater);
        QEventLoop loop;
        connect(m_instance,&ClientSideProxy::ready,&loop,&QEventLoop::quit);
        connect(m_instance,&ClientSideProxy::terminate,&loop,&QEventLoop::quit);
        m_thread->start();
        loop.exec();
    }
    return m_instance;
}

ClientSideProxy::~ClientSideProxy()
{
    ClientSideProxy::m_instance=0;
}

void ClientSideProxy::setRequestBuilder(RequestBuilder *builder)
{
    m_request_builder=builder;
}

bool ClientSideProxy::tryConnect()
{
    QMetaObject::invokeMethod(this,"connectToHost",Qt::BlockingQueuedConnection);
    if(m_socket->state()!=QTcpSocket::ConnectedState)
        return false;
    return true;
}

void ClientSideProxy::connectToHost()
{
    m_socket->connectToHost(m_host_port.at(0),m_host_port.at(1).toInt());
    if(!m_socket->waitForConnected())
        return;
    //m_socket->setReadBufferSize(SOCKET_BUFFER_SIZE);
    m_socket_data_stream.setDevice(m_socket);
    m_raw_buffer.open(QIODevice::ReadWrite);
}

QAbstractSocket::SocketState ClientSideProxy::state()
{
    if(m_socket)
        return m_socket->state();
}

void ClientSideProxy::run()
{
    m_config_file=new QFile(CONFIG_FILE,this);
    if(!m_config_file->open(QIODevice::ReadOnly))
    {
        m_instance=0;
        emit terminate();
        return;
    }
    QTextStream stream(m_config_file);
    QString line=stream.readAll();
    m_host_port=line.split(":");
    m_socket=new QTcpSocket(this);
    connect(m_socket,&QTcpSocket::readyRead,this,&ClientSideProxy::onReadyRead);
    connect(m_socket,&QTcpSocket::disconnected,this,&ClientSideProxy::onDisconnected);
    emit ready();
}

void ClientSideProxy::sendData(quint32 command, QByteArray data)
{
    log("sendData()");
    PacketHeader header;
    header.command=command;
    header.data_length=data.size();
    header.start_flag=PACKET_START;
    header.status=CMD_OK;
    header.uid=QDateTime::currentMSecsSinceEpoch();
    m_socket_data_stream<<header;
    m_socket->waitForBytesWritten();
    QBuffer buffer(&data);
    Q_ASSERT(buffer.open(QIODevice::ReadOnly));
    qint64 written=0;
    qint64 portions=0;
    while(!buffer.atEnd())
    {
        written+=m_socket->write(buffer.read(BLOCK_SIZE));
        m_socket->waitForBytesWritten();
        portions++;
    }
    buffer.close();
    qDebug()<<written<<" bytes written in "<<portions<<" portions of "<<data.size();
    m_queue.enqueue(header);
    if(!m_timer.isActive())
    {
        m_timer.setSingleShot(true);
        m_timer.start(10000);
    }
}

void ClientSideProxy::onDisconnected()
{
    //m_instance=0;
    //emit terminate();
}

void ClientSideProxy::disconnectFromServer()
{
    m_socket->disconnectFromHost();
    m_raw_buffer.buffer().clear();
    m_raw_buffer.close();
}

void ClientSideProxy::timeout()
{
    m_socket->readAll();
    m_raw_buffer.seek(0);
    m_raw_buffer.buffer().clear();
    m_packet_started=false;
    PacketHeader header=m_queue.dequeue();
    header.data_length=0;
    header.status=CMD_TIMEOUT;
    QMetaObject::invokeMethod(m_request_builder,CALLBACK,Qt::BlockingQueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QBufferPtr,&m_raw_buffer));
    /*if(m_methods.contains(header.command))
    {
        QString method=m_methods[m_header.command];
        log("Timeout invokeMethod "+method);
        QMetaObject::invokeMethod(m_request_builder,method.toStdString().c_str(),Qt::BlockingQueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QBufferPtr,&m_raw_buffer));
    }*/
    log("Timeout");
}
