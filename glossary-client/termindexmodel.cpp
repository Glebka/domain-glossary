#include "termindexmodel.h"

TermIndexModel::TermIndexModel(RequestBuilder *builder, QMap<quint32, DomainInfo> *domains, QObject *parent) :
    QAbstractListModel(parent)
  ,m_request(builder)
  ,is_requested(false)
  ,can_fetch(true)
{
    m_request->startTransaction();
    m_request->getAllDomains();
    QByteArray bytes=m_request->endTransaction();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    QList<DomainInfo> list;
    stream>>list;
    foreach (DomainInfo di, list) {
        m_domains.insert(di.id,di);
    }
    startTimer(1000);
    connect(m_request,&RequestBuilder::loadTerms,this,&TermIndexModel::onTermsFetched);
    //connect(this,&TermIndexModel::request,this,&TermIndexModel::onRequest);
}

void TermIndexModel::onTermsFetched(QList<TermInfo> terms)
{
    /*QSet<TermInfo> old=m_terms.toSet();
    QSet<TermInfo> newTerms=terms.toSet();
    QSetnewTerms.subtract(old).toList()*/

    beginInsertRows(QModelIndex(),m_terms.size(),m_terms.size()+terms.size());
    m_terms.append(terms);
    endInsertRows();
    is_requested=false;

    if(terms.size()==PORTION_SIZE)
        can_fetch=true;
}

QModelIndex TermIndexModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if(m_terms.size()>row)
    {
        TermInfo * ti=const_cast<TermInfo *>(&m_terms.at(row));
        return createIndex(row, column, ti);
    }
    return QModelIndex();
}

int TermIndexModel::rowCount(const QModelIndex &parent) const
{
    QModelIndex index;
    if(m_terms.size()==0)
        request();
    return m_terms.size();
}

QVariant TermIndexModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    TermInfo *item = static_cast<TermInfo*>(index.internalPointer());
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        if(m_domains.contains(item->domain_id))
            return item->title+" ("+m_domains.value(item->domain_id).title.toLower()+")";
        else
            return item->title;
    case Qt::UserRole:
        return item->id;
    default:
        return QVariant();
    }
}

void TermIndexModel::fetchMore(const QModelIndex &parent)
{
    if(parent.isValid())
        return;
    request();
    can_fetch=false;
}

bool TermIndexModel::canFetchMore(const QModelIndex &parent) const
{
    if(parent.isValid())
        return false;
    return can_fetch;
}

Qt::ItemFlags TermIndexModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemNeverHasChildren;
}

void TermIndexModel::request() const
{
    if(is_requested)
        return;
    //m_request->startTransaction();
    m_request->getAllTerms(m_terms.size(),PORTION_SIZE);
    is_requested=true;
    /*QByteArray array=m_request->endTransaction();
    QDataStream stream(&array,QIODevice::ReadOnly);
    QList<TermInfo> list;*/
    //stream>>list;
    //onTermsFetched(list);
}


void TermIndexModel::timerEvent(QTimerEvent *a)
{
    can_fetch=true;
    a->accept();
}
