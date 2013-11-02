#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QStringList>
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

struct DomainInfo
{
    quint32 id;
    QString title;
};

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

struct TermDefinition
{
    TermInfo termInfo;
    ConceptInfo conceptInfo;
    QString domain;
    QString title;
    QStringList keywords;
    QString text;
};

typedef QList<TermInfo> TermList;
typedef QList<DomainInfo> DomainList;

#define ON_GET_USER_BY_ID   "onGetUserById"
#define ON_LOGIN            "onLogin"
#define ON_GET_ALL_TERMS    "onGetAllTerms"
#define ON_GET_ALL_DOMAINS  "onGetAllDomains"

const quint32 TERM_INFO_START=0x100;
const quint32 TERM_INFO_END=0x101;

const quint32 CONCEPT_INFO_START=0x102;
const quint32 CONCEPT_INFO_END=0x103;

const quint32 DOMAIN_INFO_START=0x104;
const quint32 DOMAIN_INFO_END=0x105;

const quint32 USER_INFO_START=0x106;
const quint32 USER_INFO_END=0x107;

const quint32 TERM_DEFINITION_START=0x108;
const quint32 TERM_DEFINITION_END=0x109;

const quint32 CMD_OK=0x200;
const quint32 CMD_ERROR=0x502;
const quint32 CMD_NOT_FOUND=0x404;
const quint32 CMD_FORBIDDEN=0x403;

const quint32 CMD_LOGIN=0x301;

QDataStream & operator <<(QDataStream &out, const TermInfo & ti);
QDataStream & operator <<(QDataStream &out, const ConceptInfo & ci);
QDataStream & operator <<(QDataStream &out, const DomainInfo & di);
QDataStream & operator <<(QDataStream &out, const UserInfo & ui);
QDataStream & operator <<(QDataStream &out, const TermDefinition & td);

QDataStream & operator >>(QDataStream &out, TermInfo & ti);
QDataStream & operator >>(QDataStream &out, ConceptInfo & ci);
QDataStream & operator >>(QDataStream &out, DomainInfo & di);
QDataStream & operator >>(QDataStream &out, UserInfo & ui);
QDataStream & operator >>(QDataStream &out, TermDefinition & td);

#endif
