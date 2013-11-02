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
#include <QDebug>

#include <common.h>

class DataProvider : public QObject
{
    Q_OBJECT

protected:

    static DataProvider * m_instance;
    static QThread * m_thread;

    DataProvider(QObject *parent = 0);
public:
    ~DataProvider();

    void configToXML();
    void contentsToXML();
    void contentsToBin();
    static DataProvider *getInstance();

signals:
    void terminate();
    void ready();

public slots:

    void run();

    void getTermDefinition(quint32 termId);
    void addTerm(quint32 domainId, QString term, quint32 conceptId);
    void addTerm(quint32 domainId, QString term, QStringList keywords, QString text);
    void editTerm(quint32 termid, quint32 conceptId, QString term, QStringList keywords, QString text);
    void requestTermDefinition(quint32 domainId, QString term);
    void login(QString user, QString password);
    void getAllTerms();
    void getTermsByDomain(quint32 domainId);
    void getAllDomains();
    void search(QString search);
    void getDomainById(quint32 domainId);
    void getUserById(quint32 userId);

private:

    bool initFromXml();
    bool initFromBinary();
    void log(QString text);
    bool initConfig();

    QMap<quint32,QString> * m_domains_by_id;
    QMap<quint32,UserInfo *> * m_users_by_id;
    QMap<QString, UserInfo *> * m_users_by_login;
    QMultiMap<quint32,UserInfo *> * m_users_by_domain;
    QMap<quint32,TermInfo *> * m_term_by_id;
    QMap<quint32,ConceptInfo *> * m_concept_by_id;
    QMultiHash<QString,TermInfo *> * m_terms_by_name;
    QMultiHash<QString,ConceptInfo *> * m_concepts_by_keyword;

    UserInfo * m_users;
    TermInfo * m_terms;
    ConceptInfo * m_concepts;

};

//DataProvider * DataProvider::m_instance=0;
//QThread * DataProvider::m_thread=0;

#endif // DATAPROVIDER_H
