#include "dataprovider.h"

QThread * DataProvider::m_thread=0;
DataProvider * DataProvider::m_instance=0;

DataProvider::DataProvider(QObject *parent)
    : QObject(parent)
    , m_concepts(0)
    , m_terms(0)
    , m_users(0)
    , m_concepts_by_keyword(0)
    , m_concept_by_id(0)
    , m_domains_by_id(0)
    , m_terms_by_name(0)
    , m_term_by_id(0)
    , m_users_by_domain(0)
    , m_users_by_id(0)
    , m_users_by_login(0)
    , m_term_by_domain_id(0)

{
    qRegisterMetaType<TermInfo>("TermInfo");
    qRegisterMetaType<ConceptInfo>("ConceptInfo");
    qRegisterMetaType<UserInfo>("UserInfo");
    qRegisterMetaType<TermDefinition>("TermDefinition");
    qRegisterMetaType<TermList>("TermList");
    qRegisterMetaType<DomainList>("DomainList");
}

DataProvider::~DataProvider()
{
    delete m_concepts_by_keyword;
    delete m_concept_by_id;
    delete m_domains_by_id;
    delete m_terms_by_name;
    delete m_term_by_id;
    delete m_users_by_domain;
    delete m_users_by_id;
    delete m_term_by_domain_id;
    if(m_users)
        delete[] m_users;
    if(m_terms)
        delete[] m_terms;
    if(m_concepts)
        delete[] m_concepts;
}

bool DataProvider::initFromXml()
{
    Q_ASSERT(initConfig());
    if(!initConfig()) return false;

    QDomDocument contents(CONTENTS_TAG);
    QFile contents_file(CONTENTS_FILE);
    if(!contents_file.open(QIODevice::ReadOnly))
    {
        log("Failed to load contents file.");
        return false;
    }
    if(!contents.setContent(&contents_file))
    {
        log("Failed to load XML from contents file.");
        return false;
    }
    contents_file.close();

    QDomElement e;
    QDomNode termsNode=contents.documentElement().firstChild();
    m_terms=new TermInfo[termsNode.childNodes().size()];

    Q_ASSERT(m_terms);
    if(!m_terms) return false;

    int index=0;
    QDomNode term=termsNode.firstChild();
    while(!term.isNull())
    {
        e=term.toElement();
        if(!e.isNull())
        {
            m_terms[index].id=e.attributes().namedItem(ID_ATTR).nodeValue().toUInt();
            m_terms[index].domain_id=e.attributes().namedItem(DOMAIN_ID_ATTR).nodeValue().toUInt();
            m_terms[index].title=e.namedItem(TITLE_TAG).toElement().text();
            QStringList list=e.namedItem(CONCEPT_LIST_TAG).toElement().text().split("|");
            foreach (QString element, list) {
                m_terms[index].concept_list.append(element.toUInt());
            }
            m_term_by_id->insert(m_terms[index].id,&m_terms[index]);
            m_terms_by_name->insertMulti(m_terms[index].title.toLower(),&m_terms[index]);
            m_term_by_domain_id->insertMulti(m_terms[index].domain_id,&m_terms[index]);
            index++;
        }
        term=term.nextSibling();
    }
    QDomNode conceptsNode=termsNode.nextSibling();
    m_concepts=new ConceptInfo[conceptsNode.childNodes().size()];

    Q_ASSERT(m_concepts);
    if(!m_concepts) return false;

    QDomNode concept=conceptsNode.firstChild();
    index=0;
    while(!concept.isNull())
    {
        e=concept.toElement();
        if(!e.isNull())
        {
            m_concepts[index].id=e.attributes().namedItem(ID_ATTR).nodeValue().toUInt();
            m_concepts[index].domain_id=e.attributes().namedItem(DOMAIN_ID_ATTR).nodeValue().toUInt();
            m_concepts[index].keywords=e.namedItem(KEYWORDS_TAG).toElement().text().split(",");
            m_concepts[index].last_modified=QDateTime::fromString(e.namedItem(LAST_MODIFIED_TAG).toElement().text(),DATE_TIME_FORMAT);
            QStringList list=e.namedItem(TERM_LIST_TAG).toElement().text().split("|");
            foreach (QString element, list) {
                m_concepts[index].term_list.append(element.toUInt());
            }
            m_concept_by_id->insert(m_concepts[index].id,&m_concepts[index]);
            foreach (QString keyword, m_concepts[index].keywords) {
                m_concepts_by_keyword->insertMulti(keyword,&m_concepts[index]);
            }
            index++;
        }
        concept=concept.nextSibling();
    }
}

bool DataProvider::initFromBinary()
{
    //Q_ASSERT(initConfig());
    if(!initConfig()) return false;

    QFile contents_file("contents.dat");
    if(!contents_file.open(QIODevice::ReadOnly))
    {
        log("Failed to load contents file.");
        return false;
    }

    QDataStream stream(&contents_file);
    stream.setVersion(QDataStream::Qt_5_1);
    quint32 index;
    quint32 size;
    stream>>size;

    m_terms=new TermInfo[size];

    Q_ASSERT(m_terms);
    if(!m_terms) return false;

    for(index=0;index<size;index++)
    {
        stream>>m_terms[index];
        m_term_by_id->insert(m_terms[index].id,&m_terms[index]);
        m_terms_by_name->insertMulti(m_terms[index].title.toLower(),&m_terms[index]);
        m_term_by_domain_id->insertMulti(m_terms[index].domain_id,&m_terms[index]);
    }
    index=0;
    stream>>size;
    m_concepts=new ConceptInfo[size];

    Q_ASSERT(m_concepts);
    if(!m_concepts) return false;

    for(index=0;index<size;index++)
    {
        stream>>m_concepts[index];
        m_concept_by_id->insert(m_concepts[index].id,&m_concepts[index]);
        foreach (QString keyword, m_concepts[index].keywords) {
            m_concepts_by_keyword->insertMulti(keyword,&m_concepts[index]);
        }

    }
    return true;
}

void DataProvider::configToXML()
{
    QFile output("config_out.xml");
    if(!output.open(QIODevice::WriteOnly))
    {
        log("Failed to open config file for writing.");
        return;
    }
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0");
    writer.writeStartElement(CONFIG_TAG);
    writer.writeStartElement(DOMAINS_TAG);
    QMapIterator<quint32,DomainInfo *> it(*m_domains_by_id);
    while(it.hasNext())
    {
        writer.writeStartElement(DOMAIN_TAG);
        writer.writeAttribute(ID_ATTR,QString::number(it.peekNext().key()));
        writer.writeCharacters(it.next().value()->title);
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeStartElement(USERS_TAG);
    QMapIterator<quint32,UserInfo *> itu(*m_users_by_id);
    while(itu.hasNext())
    {
        quint32 key=itu.peekNext().key();
        const UserInfo * ui=itu.next().value();
        writer.writeStartElement(USER_TAG);
        writer.writeAttribute(ID_ATTR,QString::number(key));
        switch(ui->type)
        {
        case User:
            writer.writeAttribute(USER_TYPE_ATTR,USER_VALUE);
            break;
        case Expert:
            writer.writeAttribute(USER_TYPE_ATTR,EXPERT_VALUE);
            writer.writeAttribute(DOMAIN_ID_ATTR,QString::number(ui->domain_id));
            break;
        default:
            log("Unknown UserType.");
        }
        writer.writeStartElement(EMAIL_TAG);
        writer.writeCharacters(ui->email);
        writer.writeEndElement();
        writer.writeStartElement(PASSWORD_TAG);
        writer.writeCharacters(ui->password);
        writer.writeEndElement();
        writer.writeStartElement(FULL_NAME_TAG);
        writer.writeCharacters(ui->full_name);
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndDocument();
    output.close();
}

void DataProvider::contentsToXML()
{
    QFile output("contents_out.xml");
    if(!output.open(QIODevice::WriteOnly))
    {
        log("Failed to open contents file for writing.");
        return;
    }

    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0");
    writer.writeStartElement(CONTENTS_TAG);
    writer.writeStartElement(TERMS_TAG);

    QMapIterator<quint32,TermInfo *> itt(*m_term_by_id);
    while(itt.hasNext())
    {
        quint32 key=itt.peekNext().key();
        const TermInfo * ti=itt.next().value();
        writer.writeStartElement(TERM_TAG);
        writer.writeAttribute(ID_ATTR,QString::number(key));
        writer.writeAttribute(DOMAIN_ID_ATTR,QString::number(ti->domain_id));
        writer.writeStartElement(TITLE_TAG);
        writer.writeCharacters(ti->title);
        writer.writeEndElement();
        writer.writeStartElement(CONCEPT_LIST_TAG);
        writer.writeCharacters(ti->concept_list.toString("|"));
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeStartElement(CONCEPTS_TAG);

    QMapIterator<quint32,ConceptInfo *> itc(*m_concept_by_id);
    while(itc.hasNext())
    {
        quint32 key=itc.peekNext().key();
        const ConceptInfo * ci=itc.next().value();
        writer.writeStartElement(CONCEPT_TAG);
        writer.writeAttribute(ID_ATTR,QString::number(key));
        writer.writeAttribute(DOMAIN_ID_ATTR,QString::number(ci->domain_id));
        writer.writeStartElement(TERM_LIST_TAG);
        writer.writeCharacters(ci->term_list.toString("|"));
        writer.writeEndElement();
        writer.writeStartElement(LAST_MODIFIED_TAG);
        writer.writeCharacters(ci->last_modified.toString(DATE_TIME_FORMAT));
        writer.writeEndElement();
        writer.writeStartElement(KEYWORDS_TAG);
        writer.writeCharacters(ci->keywords.join(","));
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndDocument();
    output.close();
}

void DataProvider::contentsToBin()
{
    QFile binout("contents.dat");
    if(!binout.open(QIODevice::WriteOnly))
    {
        log("Failed to open contents binary file for writing.");
        return;
    }
    QDataStream stream(&binout);
    stream.setVersion(QDataStream::Qt_5_1);

    stream<<(quint32)m_term_by_id->size();
    QMapIterator<quint32,TermInfo *> itt(*m_term_by_id);
    const TermInfo * ti;
    while(itt.hasNext())
    {
        ti=itt.next().value();
        stream<<(*ti);
    }
    stream<<(quint32)m_concept_by_id->size();
    QMapIterator<quint32,ConceptInfo *> itc(*m_concept_by_id);
    const ConceptInfo * ci;
    while(itc.hasNext())
    {
        ci=itc.next().value();
        stream<<(*ci);
    }
    binout.close();
}

DataProvider *DataProvider::getInstance()
{
    if(!m_instance)
    {
        m_instance=new DataProvider();
        m_thread=new QThread();
        m_instance->moveToThread(m_thread);
        connect(m_thread,&QThread::started,m_instance,&DataProvider::run);
        connect(m_instance,&DataProvider::terminate,m_thread,&QThread::quit);
        connect(m_thread,&QThread::finished,m_thread,&QThread::deleteLater);
        connect(m_thread,&QThread::finished,m_instance,&DataProvider::deleteLater);
        QEventLoop loop;
        connect(m_instance,&DataProvider::ready,&loop,&QEventLoop::quit);
        m_thread->start();
        loop.exec();
        m_instance->log("Initialized");
    }
    return m_instance;
}

const QMap<quint32, DomainInfo *> *DataProvider::domainsById()
{
    return m_domains_by_id;
}

const QMap<quint32, UserInfo *> *DataProvider::usersById()
{
    return m_users_by_id;
}

const QMap<QString, UserInfo *> *DataProvider::usersByLogin()
{
    return m_users_by_login;
}

const QMultiMap<quint32, UserInfo *> *DataProvider::usersByDomain()
{
    return m_users_by_domain;
}

const QMap<quint32, DomainInfo *> *DataProvider::allDomains()
{
    return m_domains_by_id;
}

const QMap<quint32, TermInfo *> *DataProvider::rdLockTermsById()
{
    m_lockers[TermById].lockForRead();
    return m_term_by_id;
}

QMap<quint32, TermInfo *> *DataProvider::wrLockTermsById()
{
    m_lockers[TermById].lockForWrite();
    return m_term_by_id;
}

const QMultiMap<quint32, TermInfo *> *DataProvider::rdLockTermsByDomainId()
{
    m_lockers[TermByDomainId].lockForRead();
    return m_term_by_domain_id;
}

QMultiMap<quint32, TermInfo *> *DataProvider::wrLockTermsByDomainId()
{
    m_lockers[TermByDomainId].lockForWrite();
    return m_term_by_domain_id;
}

const QMap<quint32, ConceptInfo *> *DataProvider::rdLockConceptById()
{
    m_lockers[ConceptById].lockForRead();
    return m_concept_by_id;
}

QMap<quint32, ConceptInfo *> *DataProvider::wrLockConceptById()
{
    m_lockers[ConceptById].lockForWrite();
    return m_concept_by_id;
}

const QMultiHash<QString, TermInfo *> *DataProvider::rdLockTermsByName()
{
    m_lockers[TermByName].lockForRead();
    return m_terms_by_name;
}

QMultiHash<QString, TermInfo *> *DataProvider::wrLockTermsByName()
{
    m_lockers[TermByName].lockForWrite();
    return m_terms_by_name;
}

const QMultiHash<QString, ConceptInfo *> *DataProvider::rdLockConceptsByKeyword()
{
    m_lockers[ConceptByKeyword].lockForRead();
    return m_concepts_by_keyword;
}

QMultiHash<QString, ConceptInfo *> *DataProvider::wrLockConceptsByKeyword()
{
    m_lockers[ConceptByKeyword].lockForWrite();
    return m_concepts_by_keyword;
}

QString DataProvider::getConceptFileContents(quint32 conceptId)
{
    quint8 * id_path=(quint8 *)&conceptId;
    QStringList pathList;
    pathList<<DATA_DIRECTORY
            <<QString::number(id_path[0],16)
            <<QString::number(id_path[1],16)
            <<QString::number(id_path[2],16)
            <<QString::number(id_path[3],16);
    QString path=pathList.join("/");
    path.append(EXTENSION);
    log("Will open concept file: "+path);
    int fdesc=open(path.toStdString().c_str(),O_RDONLY);
    if(fdesc==-1)
    {
        log("Can't open concept file: "+path);
        return QString();
    }
    flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    int lockRes=fcntl(fdesc,F_SETLKW,&lock);
    if(lockRes==-1)
    {
        log("Can't set RDLOCK to concept file: "+path);
        close(fdesc);
        return QString();
    }
    QFile file;
    Q_ASSERT(file.open(fdesc,QIODevice::ReadOnly));
    QTextStream textStream(&file);
    QString content=textStream.readAll();
    file.close();
    close(fdesc);
    return content;
}

void DataProvider::lockRead()
{
    for(int i=0;i<4;i++)
        m_lockers[i].lockForRead();
}

void DataProvider::lockWrite()
{
    for(int i=0;i<4;i++)
        m_lockers[i].lockForWrite();
}


void DataProvider::unlock()
{
    for(int i=0;i<LOCKERS_COUNT;i++)
        m_lockers[i].unlock();
}

void DataProvider::run()
{
    qDebug()<<"Running...";
    m_concepts_by_keyword=new QMultiHash<QString,ConceptInfo *>();
    m_concept_by_id=new QMap<quint32,ConceptInfo *>();
    m_domains_by_id=new QMap<quint32,DomainInfo *>();
    m_terms_by_name=new QMultiHash<QString,TermInfo *>();
    m_term_by_id=new QMap<quint32,TermInfo *>();
    m_users_by_domain=new QMultiMap<quint32,UserInfo *>();
    m_users_by_id=new QMap<quint32,UserInfo *>();
    m_users_by_login=new QMap<QString,UserInfo *>();
    m_term_by_domain_id=new QMultiMap<quint32,TermInfo *>();
    //if(initFromBinary())
    if(initFromXml())
        emit ready();
}

void DataProvider::log(QString text)
{
    qDebug()<<"[DataProvider] "<<text;
}

bool DataProvider::initConfig()
{
    QDomDocument config(CONFIG_TAG);
    QFile config_file(CONFIG_FILE);
    if(!config_file.open(QIODevice::ReadOnly))
    {
        log("Failed to load config file.");
        return false;
    }
    if(!config.setContent(&config_file))
    {
        log("Failed to load XML from config file.");
        return false;
    }
    config_file.close();

    // loading config

    QDomNode domains = config.documentElement().firstChild();
    QDomNode domain=domains.firstChild();
    QDomElement e;
    while(!domain.isNull())
    {
        e = domain.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            DomainInfo * di=new DomainInfo;
            di->id=e.attributes().namedItem(ID_ATTR).nodeValue().toUInt();
            di->title=e.text();
            m_domains_by_id->insert(di->id,di);
            //qDebug()<<di->title;
            //log(e.attributes().namedItem(ID_ATTR).nodeValue());
            //log(e.text());
        }
        domain=domain.nextSibling();
    }

    QDomNode usersNode=domains.nextSibling();
    QDomNode user=usersNode.firstChild();
    m_users=new UserInfo[usersNode.childNodes().size()];
    int index=0;
    while(!user.isNull())
    {
        e = user.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            m_users[index].id=e.attributes().namedItem(ID_ATTR).nodeValue().toUInt();
            if(e.attributes().contains(DOMAIN_ID_ATTR))
            {
                m_users[index].domain_id=e.attributes().namedItem(DOMAIN_ID_ATTR).nodeValue().toInt();
                m_users[index].type=Expert;
            }
            else
            {
                m_users[index].type=User;
                m_users[index].domain_id=0;
            }
            m_users[index].email=e.namedItem(EMAIL_TAG).toElement().text();
            m_users[index].password=e.namedItem(PASSWORD_TAG).toElement().text();
            m_users[index].full_name=e.namedItem(FULL_NAME_TAG).toElement().text();
            m_users_by_id->insert(m_users[index].id,&m_users[index]);
            m_users_by_domain->insertMulti(m_users[index].domain_id,&m_users[index]);
            m_users_by_login->insert(m_users[index].email,&m_users[index]);
            index++;
        }
        user=user.nextSibling();
    }
    return true;
}

QString IdList::toString(QString separator) const
{
    QStringList list;
    QListIterator<quint32> it(*this);
    while(it.hasNext())
    {
        list<<QString::number(it.next());
    }
    return list.join(separator);
}
