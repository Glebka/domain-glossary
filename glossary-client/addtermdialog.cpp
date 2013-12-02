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
    setWindowTitle("Добавить термин");
}

AddTermDialog::~AddTermDialog()
{
    delete ui;
}


void AddTermDialog::accept()
{
    if(ui->txtTerm->text().length()==0)
    {
        QMessageBox::warning(this,"Добавление термина","Название термина не должно быть пустым.");
        return;
    }
    if(ui->radioConnect->isChecked() && ui->comboBox->currentIndex()==-1)
    {
        QMessageBox::warning(this,"Добавление термина","Выберите термин из выпадающего списка.");
        return;
    }
    m_request->startTransaction();
    if(ui->radioConnect->isChecked())
        m_request->addTermToExisting(ui->txtTerm->text(),ui->comboBox->itemData(ui->comboBox->currentIndex(),Qt::UserRole).toUInt());
    else
        m_request->addTerm(ui->txtTerm->text());
    QByteArray bytes=m_request->endTransaction().buffer();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    if(bytes.size()>0)
    {
        TermInfo ti;
        stream>>ti;
        qDebug()<<ti.title;
        QDialog::accept();
    }
    else
        QMessageBox::critical(this,"Добавление термина","Не удалось добавить новый термин.");
}
