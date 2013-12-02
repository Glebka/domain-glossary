#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMultiHash>
#include <QMultiMap>
#include <QMapIterator>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QDataStream>
#include <QThread>
#include <QEventLoop>
#include <QReadWriteLock>
#include <QTextStream>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <common.h>

enum Method {TermById,TermByDomainId,TermByName,ConceptById,ConceptByKeyword};

#define LOCKERS_COUNT 5

class DataProvider : public QObject
{
    Q_OBJECT

private:

    static DataProvider * m_instance;
    static QThread * m_thread;

    DataProvider(QObject *parent = 0);
    Q_DISABLE_COPY(DataProvider)

public:
    ~DataProvider();

    void configToXML();
    void contentsToXML();
    void contentsToBin();
    static DataProvider *getInstance();

    // readonly data access methods

    const QMap<quint32,DomainInfo *> * domainsById();
    const QMap<quint32,UserInfo *> * usersById();
    const QMap<QString, UserInfo *> * usersByLogin();
    const QMultiMap<quint32,UserInfo *> * usersByDomain();
    const QMap<quint32,DomainInfo *> * allDomains();


    // data access methods

    const QMap<quint32,TermInfo *> * rdLockTermsById();
    QMap<quint32,TermInfo *> * wrLockTermsById();

    const QMultiMap<quint32,TermInfo *> * rdLockTermsByDomainId();
    QMultiMap<quint32,TermInfo *> * wrLockTermsByDomainId();

    const QMap<quint32,ConceptInfo *> * rdLockConceptById();
    QMap<quint32,ConceptInfo *> * wrLockConceptById();

    const QMultiMap<QString,TermInfo *> * rdLockTermsByName();
    QMultiMap<QString,TermInfo *> * wrLockTermsByName();

    const QMultiHash<QString,ConceptInfo *> * rdLockConceptsByKeyword();
    QMultiHash<QString,ConceptInfo *> * wrLockConceptsByKeyword();
    
    QString getConceptFileContents(quint32 conceptId);
    
    void unlock();

signals:
    void terminate();
    void ready();

public slots:

    void run();

private:

    bool initFromXml();
    bool initFromBinary();
    void log(QString text);
    bool initConfig();

    void lockRead();
    void lockWrite();

    QMap<quint32,DomainInfo *> * m_domains_by_id;
    QMap<quint32,UserInfo *> * m_users_by_id;
    QMap<QString, UserInfo *> * m_users_by_login;
    QMultiMap<quint32,UserInfo *> * m_users_by_domain;


    QMap<quint32,TermInfo *> * m_term_by_id;
    QMultiMap<quint32,TermInfo *> * m_term_by_domain_id;
    QMap<quint32,ConceptInfo *> * m_concept_by_id;
    QMultiMap<QString,TermInfo *> * m_terms_by_name;
    QMultiHash<QString,ConceptInfo *> * m_concepts_by_keyword;

    UserInfo * m_users;
    TermInfo * m_terms;
    ConceptInfo * m_concepts;

    QReadWriteLock m_lockers[LOCKERS_COUNT];
};

//DataProvider * DataProvider::m_instance=0;
//QThread * DataProvider::m_thread=0;

#endif // DATAPROVIDER_H
