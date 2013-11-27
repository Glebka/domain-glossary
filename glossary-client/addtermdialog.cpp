#include "addtermdialog.h"
#include "ui_addtermdialog.h"

AddTermDialog::AddTermDialog(RequestBuilder *builder, QAbstractItemModel *model,QMap<quint32,DomainInfo> * domains, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTermDialog)
  ,m_request(builder)
  ,m_model(model)
  ,m_domains(domains)
{
    ui->setupUi(this);
    ui->comboBox->setModel(model);
    ui->txtDomain->setText(m_domains->value(builder->user.domain_id).title);
}

AddTermDialog::~AddTermDialog()
{
    delete ui;
}
