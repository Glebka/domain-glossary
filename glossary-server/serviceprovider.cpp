#include <serviceprovider.h>

void ServiceProvider::login(PacketHeader header, QBufferPtr data)
{
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);

    Q_ASSERT(data->isOpen());

    QString login,password;
    input>>login;
    input>>password;
    UserInfo * ui_ptr=m_provider->usersByLogin()->value(login,0);
    if(!ui_ptr)
        header.status=CMD_FORBIDDEN;
    else
    {
        if(ui_ptr->password!=password)
            header.status=CMD_FORBIDDEN;
        else
        {
            header.status=CMD_OK_NOCACHE;
            output<<*ui_ptr;
            m_user=ui_ptr;
        }
    }
    invokeCallBack(header,result);
}

void ServiceProvider::getAllDomains(PacketHeader header,QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }

    QDataStream input(data);
    Q_ASSERT(data->isOpen());

    QBuffer result;
    result.open(QIODevice::WriteOnly);
    QDataStream output(&result);

    //Q_ASSERT(data.isOpen());
    QList<DomainInfo> list;
    QMultiMap<QString,DomainInfo> map;
    foreach (DomainInfo * di, m_provider->allDomains()->values()) {
        map.insert(di->title,*di);
    }
    output<<map.values();
    header.status=CMD_OK;
    invokeCallBack(header,result.buffer());
    result.close();
}

void ServiceProvider::getAllTerms(PacketHeader header,QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream output(&result,QIODevice::WriteOnly);

    QDataStream input(data);
    Q_ASSERT(data->isOpen());

    quint32 start=0,length=0;
    input>>start>>length;
    auto termsByName=m_provider->rdLockTermsByName();
    //QList<TermInfo> list;
    QMultiMap<QString,TermInfo> map;
    foreach (TermInfo * ti, termsByName->values().mid(start,length)) {
        map.insert(ti->title,*ti);
    }
    m_provider->unlock();
    output<<map.values();
    header.status=CMD_OK;
    invokeCallBack(header,result);
}

void ServiceProvider::getTermsByDomain(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 domainId=0;
    input>>domainId;
    const QMultiMap<quint32,TermInfo *> * termsByDomain=m_provider->rdLockTermsByDomainId();
    QMultiMap<QString,TermInfo> map;
    foreach (TermInfo * ti, termsByDomain->values(domainId)) {
        map.insert(ti->title,*ti);
    }
    m_provider->unlock();
    output<<map.values();
    header.status=CMD_OK;
    invokeCallBack(header,result);
}

void ServiceProvider::getTerm(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 termId=0;
    input>>termId;
    auto * termsById=m_provider->rdLockTermsById();
    //auto * conceptsById=m_provider->rdLockConceptById();

    const TermInfo * ti=termsById->value(termId,0);
    if(!ti)
    {
        log("Term not found...");
        header.status=CMD_NOT_FOUND;
        m_provider->unlock();
        invokeCallBack(header);
        return;
    }
    output<<*ti;
    header.status=CMD_OK;
    m_provider->unlock();
    invokeCallBack(header,result);
}

void ServiceProvider::getConcept(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 conceptId=0;
    input>>conceptId;
    qDebug()<<"Get Concept #"<<conceptId;
    auto conceptsById=m_provider->rdLockConceptById();
    const ConceptInfo * ci=conceptsById->value(conceptId);
    if(!ci)
    {
        log("Concept not found...");
        header.status=CMD_NOT_FOUND;
        m_provider->unlock();
        invokeCallBack(header);
        return;
    }
    output<<*ci;
    header.status=CMD_OK;
    m_provider->unlock();
    invokeCallBack(header,result);
}

void ServiceProvider::getConceptText(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 conceptId=0;
    input>>conceptId;
    auto conceptsById=m_provider->rdLockConceptById();
    const ConceptInfo * ci=conceptsById->value(conceptId);
    if(!ci)
    {
        log("Concept not found...");
        header.status=CMD_NOT_FOUND;
        m_provider->unlock();
        invokeCallBack(header);
        return;
    }
    output<<m_provider->getConceptFileContents(conceptId);
    header.status=CMD_OK;
    m_provider->unlock();
    invokeCallBack(header,result);
}

void ServiceProvider::search(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    QString search;
    input>>search;
    const QMultiMap<QString, TermInfo *> * termsByName=m_provider->rdLockTermsByName();
    QSet<quint32> searchResult;
    foreach (TermInfo * ti, termsByName->values(search)) {
        searchResult<<ti->id;
    }
    m_provider->unlock();
    const QMultiHash<QString, ConceptInfo *> * conceptsByKeyword=m_provider->rdLockConceptsByKeyword();
    foreach (ConceptInfo * ci, conceptsByKeyword->values(search.toLower())) {
        searchResult+=ci->term_list.toSet();
    }
    m_provider->unlock();
    output<<searchResult;
    header.status=CMD_OK;
    invokeCallBack(header,result);
}

void ServiceProvider::addTerm(PacketHeader header, QBufferPtr data)
{
    if(!m_user || m_user->type!=Expert || m_user->domain_id==0)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    QString term;
    input>>term;
    ConceptInfo * ci=new ConceptInfo;
    TermInfo * ti=new TermInfo;
    ti->domain_id=m_user->domain_id;
    ti->title=term;
    ci->domain_id=m_user->domain_id;
    ci->last_modified=QDateTime::currentDateTime();
    QMap<quint32,TermInfo *> * termById=m_provider->wrLockTermsById();
    QMultiMap<QString,TermInfo *> * termsByName=m_provider->wrLockTermsByName();
    QMultiMap<quint32,TermInfo *> * termsByDomainId=m_provider->wrLockTermsByDomainId();
    QMap<quint32,ConceptInfo *> *conceptById=m_provider->wrLockConceptById();
    ti->id=termById->keys().last()+1;
    ci->id=conceptById->keys().last()+1;
    ti->concept_list.append(ci->id);
    ci->term_list.append(ti->id);
    termById->insert(ti->id,ti);
    termsByDomainId->insert(ti->domain_id,ti);
    termsByName->insert(ti->title,ti);
    conceptById->insert(ci->id,ci);
    output<<*ti;
    m_provider->unlock();
    header.status=CMD_OK_NOCACHE;
    invokeCallBack(header,result);
}

void ServiceProvider::addTermToExisting(PacketHeader header, QBufferPtr data)
{
    if(!m_user || m_user->type!=Expert || m_user->domain_id==0)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    QString term;
    quint32 anotherTermId;
    input>>term>>anotherTermId;
    TermInfo * ti=new TermInfo;
    ti->domain_id=m_user->domain_id;
    ti->title=term;
    QMap<quint32,TermInfo *> * termById=m_provider->wrLockTermsById();
    QMultiMap<QString,TermInfo *> * termsByName=m_provider->wrLockTermsByName();
    QMultiMap<quint32,TermInfo *> * termsByDomainId=m_provider->wrLockTermsByDomainId();
    ti->id=termById->keys().last()+1;
    ti->concept_list.append(termById->value(anotherTermId)->concept_list);
    termById->insert(ti->id,ti);
    termsByDomainId->insert(ti->domain_id,ti);
    termsByName->insert(ti->title,ti);
    output<<*ti;
    m_provider->unlock();
    header.status=CMD_OK_NOCACHE;
    invokeCallBack(header,result);
}

void ServiceProvider::editTerm(PacketHeader header, QBufferPtr data)
{
    if(!m_user || m_user->type!=Expert || m_user->domain_id==0)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    QString term,text;
    quint32 termId;
    QStringList keywords;
    input>>termId>>term>>keywords>>text;
    QMap<quint32,TermInfo *> * termById=m_provider->wrLockTermsById();
    QMultiMap<QString,TermInfo *> * termsByName=m_provider->wrLockTermsByName();
    QMap<quint32,ConceptInfo *> *conceptById=m_provider->wrLockConceptById();
    QMultiHash<QString, ConceptInfo *> * conceptsByKeyword=m_provider->wrLockConceptsByKeyword();
    TermInfo * ti=termById->value(termId);
    if(!ti)
    {
        m_provider->unlock();
        header.status=CMD_NOT_FOUND;
        log("Term not found...");
        invokeCallBack(header);
        return;
    }
    if(ti->concept_list.size()==0)
    {
        m_provider->unlock();
        header.status=CMD_NOT_FOUND;
        log("Term has no concepts...");
        invokeCallBack(header);
        return;
    }
    ConceptInfo * ci=conceptById->value(ti->concept_list.first());
    if(!ci)
    {
        m_provider->unlock();
        header.status=CMD_NOT_FOUND;
        log("Concept not found...");
        invokeCallBack(header);
        return;
    }
    if(!termsByName->remove(ti->title,ti))
        log("Can't remove term record by name: "+ti->title);
    foreach (QString key, ci->keywords) {
        if(!conceptsByKeyword->remove(key,ci))
            log("Can't remove concept record by key: "+key);
    }
    ti->title=term;
    ci->keywords=keywords;
    ci->last_modified=QDateTime::currentDateTime();
    termsByName->insert(ti->title,ti);
    foreach (QString key, ci->keywords) {
        conceptsByKeyword->insert(key,ci);
    }
    if(!m_provider->putConceptFileContents(ci->id,text))
    {
        log("Can't put new concept content for term: "+ti->title);
        m_provider->unlock();
        header.status=CMD_ERROR;
        invokeCallBack(header);
        return;
    }
    output<<*ti;
    m_provider->unlock();
    header.status=CMD_OK_NOCACHE;
    invokeCallBack(header,result);
}

void ServiceProvider::getDomainById(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 domainId=0;
    input>>domainId;
    const DomainInfo * di=m_provider->domainsById()->value(domainId,0);
    if(!di)
    {
        log("Domain not found...");
        header.status=CMD_NOT_FOUND;
        invokeCallBack(header);
        return;
    }
    output<<*di;
    header.status=CMD_OK;
    invokeCallBack(header,result);
}

void ServiceProvider::getUserById(PacketHeader header, QBufferPtr data)
{
    if(!m_user)
    {
        header.status=CMD_FORBIDDEN;
        invokeCallBack(header);
        return;
    }
    QByteArray result;
    QDataStream input(data);
    QDataStream output(&result,QIODevice::WriteOnly);
    Q_ASSERT(data->isOpen());
    quint32 userId=0;
    input>>userId;
    const UserInfo * ui=m_provider->usersById()->value(userId,0);
    if(!ui)
    {
        log("User not found...");
        header.status=CMD_NOT_FOUND;
        invokeCallBack(header);
        return;
    }
    UserInfo userInfo=*ui;
    userInfo.password="";
    output<<userInfo;
    header.status=CMD_OK;
    invokeCallBack(header,result);
}

void ServiceProvider::log(QString text)
{
    qDebug()<<"[ServiceProvider] "<<text;
}

void ServiceProvider::invokeCallBack(PacketHeader &header, QByteArray &result)
{
    QMetaObject::invokeMethod(m_worker,CALLBACK_FUNCTION,Qt::QueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QByteArray,result));
}

void ServiceProvider::invokeCallBack(PacketHeader &header)
{
    QMetaObject::invokeMethod(m_worker,CALLBACK_FUNCTION,Qt::QueuedConnection,Q_ARG(PacketHeader,header),Q_ARG(QByteArray,QByteArray()));
}

ServiceProvider::ServiceProvider()
    : m_provider(DataProvider::getInstance())
    , m_user(0)
    , m_worker(0)
{

}

void ServiceProvider::setWorker(ClientWorker *worker)
{
    m_worker=worker;
}
