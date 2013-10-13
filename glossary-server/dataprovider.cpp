#include "dataprovider.h"

DataProvider::DataProvider(QObject *parent) :
    QObject(parent),
    m_concepts(0),
    m_terms(0),
    m_users(0)
{
    m_concepts_by_keyword=new QMultiHash<QString,ConceptInfo *>();
    m_concept_by_id=new QMap<quint32,ConceptInfo *>();
    m_domains_by_id=new QMap<quint32,QString>();
    m_terms_by_name=new QMultiHash<QString,TermInfo *>();
    m_term_by_id=new QMap<quint32,TermInfo *>();
    m_users_by_domain=new QMultiMap<quint32,UserInfo *>();
    m_users_by_id=new QMap<quint32,UserInfo *>();
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
    if(m_users)
        delete[] m_users;
    if(m_terms)
        delete[] m_terms;
    if(m_concepts)
        delete[] m_concepts;
}

bool DataProvider::init()
{
    QDomDocument contents(CONTENTS_TAG);
    QDomDocument config(CONFIG_TAG);
    QFile config_file(CONFIG_FILE);
    QFile contents_file(CONTENTS_FILE);

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
    config_file.close();
    contents_file.close();

    // loading config

    QDomNode domains = config.documentElement().firstChild();
    /*if(!domains.hasChildNodes() || domains.nodeName()!=DOMAINS_TAG)
    {
        log("Invalid XML. No domains data.");
        return false;
    }*/
    QDomNode domain=domains.firstChild();
    QDomElement e;
    while(!domain.isNull())
    {
        e = domain.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            m_domains_by_id->insert(e.attributes().namedItem(ID_ATTR).nodeValue().toUInt(),e.text());
            log(e.attributes().namedItem(ID_ATTR).nodeValue());
            log(e.text());
        }
        domain=domain.nextSibling();
    }

    QDomNode usersNode=domains.nextSibling();
    /*if(!users.hasChildNodes() || users.nodeName()!=USERS_TAG)
    {
        log("Invalid XML. No users data.");
        return false;
    }*/
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
            index++;
        }
        user=user.nextSibling();
    }
    QDomNode termsNode=contents.documentElement().firstChild();
    m_terms=new TermInfo[termsNode.childNodes().size()];
    index=0;
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
            index++;
        }
        term=term.nextSibling();
    }
    QDomNode conceptsNode=termsNode.nextSibling();
    ConceptInfo * m_concepts=new ConceptInfo[conceptsNode.childNodes().size()];
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
    QMapIterator<quint32,QString> it(*m_domains_by_id);
    while(it.hasNext())
    {
        writer.writeStartElement(DOMAIN_TAG);
        writer.writeAttribute(ID_ATTR,QString::number(it.peekNext().key()));
        writer.writeCharacters(it.next().value());
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

void DataProvider::log(QString text)
{
    qDebug()<<"[DataProvider] "<<text;
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