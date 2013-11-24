#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QDebug>
#include <QObject>
#include <QTimerEvent>
#include <QTcpSocket>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QTimer>

#include <common.h>
#include <serviceprovider.h>

class ServiceProvider;

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(qintptr socketHandle,ServiceProvider * service,QObject *parent = 0);
    ~ClientWorker();

public slots:

    void run();
    void onReadyRead();
    void onDisconnected();
    void dataArrived();
    void resultReady(PacketHeader header, QByteArray result);
    void timeout();

signals:

    void finished();
    void disconnect(ClientWorker * worker);

private:

    void log(QString text);
    qint64 readData();
    void sendResponse(QByteArray & data);
    void sendResponse();

    qintptr m_handle;

    qint64 m_readed;
    qint64 m_portions;

    QTcpSocket * m_socket;
    QDataStream m_socket_data_stream;
    QBuffer m_raw_buffer;
    PacketHeader m_header;
    bool m_packet_started;
    QMap<quint32,QString> m_methods;
    ServiceProvider * m_service;
    QTimer m_timer;
};

#endif // CLIENTWORKER_H
