#ifndef CLIENTSIDEPROXY_H
#define CLIENTSIDEPROXY_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QEventLoop>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QBuffer>
#include <QByteArray>
#include <QMap>
#include <QQueue>
#include <QTimer>
#include <QDebug>

#include <common.h>
#include <requestbuilder.h>


#define CONFIG_FILE "connect.conf"

class RequestBuilder;

class ClientSideProxy : public QObject
{
    Q_OBJECT
public:
    static ClientSideProxy * getInstance();
    ~ClientSideProxy();
    void setRequestBuilder(RequestBuilder * builder);
    QTcpSocket::SocketState state();
    bool tryConnect();
signals:
    void terminate();
    void ready();

public slots:
    void connectToHost();
    void run();
    void sendData(quint32 command, QByteArray data);
    void onReadyRead();
    void onDisconnected();
    void disconnectFromServer();
    void timeout();

private:

    static ClientSideProxy * m_instance;
    static QThread * m_thread;
    QTcpSocket * m_socket;
    QFile * m_config_file;

    QDataStream m_socket_data_stream;
    QBuffer m_raw_buffer;
    PacketHeader m_header;
    bool m_packet_started;
    QMap<quint32,QString> m_methods;
    QQueue<PacketHeader> m_queue;
    QTimer m_timer;
    RequestBuilder * m_request_builder;
    QStringList m_host_port;

    qint64 m_readed;
    qint64 m_portions;

    ClientSideProxy();
    Q_DISABLE_COPY(ClientSideProxy)

    void log(QString text);
    quint64 readData();
    void dataArrived();
};

#endif // CLIENTSIDEPROXY_H
