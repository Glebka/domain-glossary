#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QDebug>
#include <QObject>
#include <QTimerEvent>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>

#include <common.h>
#include <dataprovider.h>

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(qintptr socketHandle,QObject *parent = 0);
signals:
    void finished();
    void disconnect(ClientWorker * worker);

    void loginRequest(QString user, QString password);

public slots:

    void run();
    void dataArrived();

    void onLogin(UserInfo ui, bool success);

private:
    qintptr m_handle;
    QTcpSocket * m_socket;
    QDataStream m_stream;
    DataProvider * m_provider;
};

#endif // CLIENTWORKER_H
