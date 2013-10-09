#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QDebug>
#include <QObject>
#include <QTimerEvent>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(qintptr socketHandle,QObject *parent = 0);
    void timerEvent(QTimerEvent *evt);
signals:
    void finished();
    void disconnect(ClientWorker * worker);
public slots:
    void run();
private:
    qintptr m_handle;
    QTcpSocket * m_socket;
};

#endif // CLIENTWORKER_H
