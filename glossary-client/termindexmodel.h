#ifndef TERMINDEXMODEL_H
#define TERMINDEXMODEL_H

#include <QAbstractListModel>
#include <QSet>
#include <QTimerEvent>
#include <requestbuilder.h>

#define PORTION_SIZE 20

class TermIndexModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TermIndexModel(RequestBuilder * builder,QObject *parent = 0);
signals:
public slots:
    void onTermsFetched(QList<TermInfo> terms);
    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    RequestBuilder * m_request;
    QList<TermInfo> m_terms;
    mutable bool is_requested;
    mutable bool can_fetch;
    void request() const;

    // QAbstractItemModel interface

    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *a);
};

#endif // TERMINDEXMODEL_H
