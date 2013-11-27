#ifndef ADDTERMDIALOG_H
#define ADDTERMDIALOG_H

#include <QDialog>
#include <requestbuilder.h>
#include <termindexmodel.h>

namespace Ui {
class AddTermDialog;
}

class AddTermDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTermDialog(RequestBuilder * builder, QAbstractItemModel *model, QMap<quint32, DomainInfo> *domains, QWidget *parent = 0);
    ~AddTermDialog();

private:
    RequestBuilder * m_request;
    QAbstractItemModel * m_model;
    QMap<quint32, DomainInfo> *m_domains;
    Ui::AddTermDialog *ui;
};

#endif // ADDTERMDIALOG_H
