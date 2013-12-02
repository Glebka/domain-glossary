#ifndef COMMON_H
#define COMMON_H

#include <QApplication>
#include <QDialog>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include <QBuffer>

#define CONFIG_FILE     "config.xml"
#define CONTENTS_FILE   "contents.xml"
#define DATA_DIRECTORY  "./data"
#define EXTENSION       ".rtf"


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

struct PacketHeader
{
    quint32 start_flag;
    qint64 uid;
    quint32 command;
    quint32 status;
    qint64 data_length;
};

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
typedef QBuffer* QBufferPtr;

const quint64 SOCKET_BUFFER_SIZE=512000;
const quint64 BLOCK_SIZE=2048;
const quint32 HEADER_LENGTH=28;

const quint32 PACKET_START=0x1;
const quint32 PACKET_END=0x2;

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
const quint32 CMD_OK_NOCACHE=0x201;
const quint32 CMD_ERROR=0x502;
const quint32 CMD_NOT_FOUND=0x404;
const quint32 CMD_FORBIDDEN=0x403;
const quint32 CMD_TIMEOUT=0x405;

const quint32 CMD_LOGIN=0x301;
const quint32 CMD_GET_ALL_DOMAINS=0x302;
const quint32 CMD_GET_ALL_TERMS=0x303;
const quint32 CMD_GET_TERMS_BY_DOMAIN=0x304;
const quint32 CMD_GET_TERM=0x305;
const quint32 CMD_GET_CONCEPT=0x306;
const quint32 CMD_GET_CONCEPT_TEXT=0x307;
const quint32 CMD_SEARCH=0x308;
const quint32 CMD_GET_DOMAIN=0x309;
const quint32 CMD_GET_USER=0x30A;

const quint32 CMD_ADD_TERM=0x30B;
const quint32 CMD_ADD_TERM_TO_EXISTING=0x30C;

QDataStream & operator <<(QDataStream &out, const TermInfo & ti);
QDataStream & operator <<(QDataStream &out, const ConceptInfo & ci);
QDataStream & operator <<(QDataStream &out, const DomainInfo & di);
QDataStream & operator <<(QDataStream &out, const UserInfo & ui);
QDataStream & operator <<(QDataStream &out, const TermDefinition & td);
QDataStream & operator <<(QDataStream &out, const PacketHeader & ph);

QDataStream & operator >>(QDataStream &out, TermInfo & ti);
QDataStream & operator >>(QDataStream &out, ConceptInfo & ci);
QDataStream & operator >>(QDataStream &out, DomainInfo & di);
QDataStream & operator >>(QDataStream &out, UserInfo & ui);
QDataStream & operator >>(QDataStream &out, TermDefinition & td);
QDataStream & operator >>(QDataStream &out, PacketHeader & ph);


#endif
