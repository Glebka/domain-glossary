#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QThread>
#include <QDebug>

#include <common.h>
#include <clientworker.h>
#include <dataprovider.h>

//ELYSIUM MIND - Lay It Down
//NOCTEMIA - The Summer Cools Down
//MELOUNGE - Fall (Erotica Lounge Love Affair Mix)
//KOOP - Island Blues

#define CALLBACK_FUNCTION "resultReady"

class ClientWorker;

class ServiceProvider : public QObject
{
    Q_OBJECT

public:
    ServiceProvider();
    void setWorker(ClientWorker * worker);
    /*
    QByteArray addTerm(QByteArray data);
    QByteArray addTermToConcept(QByteArray data);
    QByteArray editTerm(QByteArray data);
    QByteArray requestTermDefinition(QByteArray data);
    */
public slots:

    void login(PacketHeader header,QBufferPtr data);
    void getAllDomains(PacketHeader header,QBufferPtr data);
    void getAllTerms(PacketHeader header,QBufferPtr data);
    void getTermsByDomain(PacketHeader header,QBufferPtr data);

    void getTerm(PacketHeader header, QBufferPtr data);
    void getConcept(PacketHeader header, QBufferPtr data);
    void getConceptText(PacketHeader header,QBufferPtr data);
    void search(PacketHeader header,QBufferPtr data);

    void addTerm(PacketHeader header,QBufferPtr data);
    void addTermToExisting(PacketHeader header,QBufferPtr data);

    void getDomainById(PacketHeader header,QBufferPtr data);
    void getUserById(PacketHeader header,QBufferPtr data);

private:
    void log(QString text);
    void invokeCallBack(PacketHeader & header,QByteArray & result);
    void invokeCallBack(PacketHeader & header);

    DataProvider * m_provider;    
    UserInfo * m_user;
    ClientWorker * m_worker;
};

#endif
