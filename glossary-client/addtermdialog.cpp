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
    qDebug()<<m_request->user.domain_id;
    m_request->startTransaction();
    m_request->getDomainById(m_request->user.domain_id);
    QByteArray array=m_request->endTransaction();
    QDataStream stream(&array,QIODevice::ReadOnly);
    DomainInfo di;
    stream>>di;
    ui->txtDomain->setText(di.title);
    setWindowTitle("Добавить термин");
    ui->comboBox->setModel(model);
}

AddTermDialog::~AddTermDialog()
{
    delete ui;
}

TermInfo AddTermDialog::dialogResult()
{
    return m_term;
}

bool AddTermDialog::isAutoStartEdit()
{
    return ui->radioNewDefinition->isChecked();
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
    QByteArray bytes=m_request->endTransaction();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    if(bytes.size()>0)
    {
        stream>>m_term;
        QDialog::accept();
    }
    else
        QMessageBox::critical(this,"Добавление термина","Не удалось добавить новый термин.");
}
