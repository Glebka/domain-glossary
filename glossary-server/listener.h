#ifndef LISTENER_H
#define LISTENER_H

#include <QTcpServer>
#include <QThread>
#include <QMap>
#include <QDebug>

#include <clientworker.h>

struct Record
{
    QThread * thread;
    ClientWorker * worker;
};

class Listener : public QTcpServer
{
    Q_OBJECT

public:
    explicit Listener(QObject *parent = 0);
public slots:
    void clientDisconnected(ClientWorker * worker);
protected:
    void incomingConnection(qintptr handle);
private:

    QMap<ClientWorker *,Record> m_clients;

};

#endif // LISTENER_H
