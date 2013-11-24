#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

#include <QObject>
#include <QString>
#include <QBuffer>
#include <QDataStream>
#include <QTcpSocket>

#include <common.h>
#include <clientsideproxy.h>

class ClientSideProxy;

class RequestBuilder : public QObject
{
    Q_OBJECT
public:
    explicit RequestBuilder(QObject *parent = 0);
    QTcpSocket::SocketState state();
    bool tryConnect();
signals:
    void loggedIn(bool succes);
    void loadDomains(QList<DomainInfo> domains);
    void loadTermsByDomain(QList<TermInfo> terms);
    void loadConcept(ConceptInfo ci);

public slots:

    void login(QString user, QString password);
    void getAllDomains();
    void getAllTermsByDomain(quint32 domainId);
    void getConcept(quint32 conceptId);


    void on_getAllDomains(PacketHeader header, QBufferPtr data);
    void on_getAllTerms(PacketHeader header, QBufferPtr data);
    void on_getConcept(PacketHeader header, QBufferPtr data);
    void on_getConceptText(PacketHeader header, QBufferPtr data);
    void on_getDomainById(PacketHeader header, QBufferPtr data);
    void on_getTerm(PacketHeader header, QBufferPtr data);
    void on_getTermsByDomain(PacketHeader header, QBufferPtr data);
    void on_getUserById(PacketHeader header, QBufferPtr data);
    void on_search(PacketHeader header, QBufferPtr data);
    void on_login(PacketHeader header, QBufferPtr data);
    void on_error(PacketHeader header, QBufferPtr data);
    void on_forbidden(PacketHeader header, QBufferPtr data);
    void on_notFound(PacketHeader header, QBufferPtr data);
    void on_timeout(PacketHeader header, QBufferPtr data);

private:
    ClientSideProxy * m_proxy;
};

#endif // REQUESTBUILDER_H
