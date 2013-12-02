#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

#include <QObject>
#include <QString>
#include <QBuffer>
#include <QDataStream>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QMessageBox>

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
    void disconnectFromHost();

    UserInfo user;

signals:
    void loggedIn(bool succes);
    void loadDomains(QList<DomainInfo> domains);
    void loadTermsByDomain(QList<TermInfo> terms);
    void loadTerms(QList<TermInfo> terms);
    void loadConcept(ConceptInfo ci);
    void loadConceptText(QString text);
    void loadSearch(QList<quint32> results);
    void loadTerm(TermInfo term);
    void termAdded(TermInfo term);

public slots:

    void startTransaction();
    QBuffer & endTransaction();
    void login(QString user, QString password);
    void getAllDomains();
    void getAllTermsByDomain(quint32 domainId);
    void getConcept(quint32 conceptId);
    void getConceptText(quint32 conceptId);
    void getAllTerms(quint32 start, quint32 length);
    void getTerm(quint32 termId);
    void search(QString search);

    void addTerm(QString term);
    void addTermToExisting(QString term, quint32 anotherTermId);

    void on_responseReady(PacketHeader header, QBufferPtr data);

    void on_getAllDomains(PacketHeader header, QByteArray data);
    void on_getAllTerms(PacketHeader header, QByteArray data);
    void on_getConcept(PacketHeader header, QByteArray data);
    void on_getConceptText(PacketHeader header, QByteArray data);
    void on_getDomainById(PacketHeader header, QByteArray data);
    void on_getTerm(PacketHeader header, QByteArray data);
    void on_getTermsByDomain(PacketHeader header, QByteArray data);
    void on_getUserById(PacketHeader header, QByteArray data);
    void on_search(PacketHeader header, QByteArray data);

    void on_addTerm(PacketHeader header, QByteArray data);

    void on_login(PacketHeader header, QByteArray data);
    void on_error(PacketHeader header, QByteArray data);
    void on_forbidden(PacketHeader header, QByteArray data);
    void on_notFound(PacketHeader header, QByteArray data);
    void on_timeout(PacketHeader header, QByteArray data);

private:

    void terminateTransaction();
    void log(QString text);

    ClientSideProxy * m_proxy;
    QBuffer m_transaction;
    QQueue<quint32> m_queue;
    bool m_is_transaction;
    QMap<quint32,QString> m_methods;
    QMutex m_mutex;
    QWaitCondition m_transactionClosed;
};

#endif // REQUESTBUILDER_H
