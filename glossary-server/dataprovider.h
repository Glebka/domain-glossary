#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QFile>
#include <QString>
#include <QMultiHash>
#include <QMultiMap>
#include <QMapIterator>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QDataStream>

#include <QDebug>

#define CONFIG_FILE     "config.xml"
#define CONTENTS_FILE   "contents.xml"
#define DATA_DIRECTORY  "./data"


// Config file tags definition

#define CONFIG_TAG          "config"
#define DOMAINS_TAG         "domains"
#define DOMAIN_TAG          "domain"
#define USERS_TAG           "users"
#define USER_TAG            "user"
#define EMAIL_TAG           "email"
#define PASSWORD_TAG        "password"
#define FULL_NAME_TAG       "full_name"


//Contents file tags definition

#define CONTENTS_TAG        "contents"
#define TERMS_TAG           "terms"
#define TERM_TAG            "term"
#define TITLE_TAG           "title"
#define CONCEPT_LIST_TAG    "concept_list"
#define CONCEPTS_TAG        "concepts"
#define CONCEPT_TAG         "concept"
#define TERM_LIST_TAG       "term_list"
#define LAST_MODIFIED_TAG   "last_modified"
#define KEYWORDS_TAG        "keywords"

#define ID_ATTR             "id"
#define DOMAIN_ID_ATTR      "domainid"
#define USER_TYPE_ATTR      "type"

#define EXPERT_VALUE        "expert"
#define USER_VALUE          "user"

#define DATE_TIME_FORMAT    "yyyy-MM-dd hh:mm:ss"


class IdList : public QList<quint32>
{
public:
    QString toString(QString separator) const;
};

enum UserType {User,Expert};

const quint32 TERM_INFO_START=0x100;
const quint32 TERM_INFO_END=0x101;
const quint32 CONCEPT_INFO_START=0x102;
const quint32 CONCEPT_INFO_END=0x103;

struct UserInfo
{
    quint32 id;
    quint32 domain_id;
    UserType type;
    QString email;
    QString password;
    QString full_name;
};

struct TermInfo
{
    quint32 id;
    quint32 domain_id;
    QString title;
    IdList concept_list;
};

struct ConceptInfo
{
    quint32 id;
    quint32 domain_id;
    IdList term_list;
    QStringList keywords;
    QDateTime last_modified;
};

class DataProvider : public QObject
{
    Q_OBJECT
public:

    explicit DataProvider(QObject *parent = 0);
    ~DataProvider();

    bool initFromXml();
    bool initFromBinary();

    void configToXML();
    void contentsToXML();
    void contentsToBin();

signals:

public slots:

private:

    void log(QString text);
    bool initConfig();

    QMap<quint32,QString> * m_domains_by_id;
    QMap<quint32,UserInfo *> * m_users_by_id;
    QMultiMap<quint32,UserInfo *> * m_users_by_domain;
    QMap<quint32,TermInfo *> * m_term_by_id;
    QMap<quint32,ConceptInfo *> * m_concept_by_id;
    QMultiHash<QString,TermInfo *> * m_terms_by_name;
    QMultiHash<QString,ConceptInfo *> * m_concepts_by_keyword;

    UserInfo * m_users;
    TermInfo * m_terms;
    ConceptInfo * m_concepts;

};

#endif // DATAPROVIDER_H
