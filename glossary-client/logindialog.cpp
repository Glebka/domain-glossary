#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(RequestBuilder *builder, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_request(builder)
    , m_result(false)
{
    ui->setupUi(this);
    connect(m_request,&RequestBuilder::loggedIn,this,&LoginDialog::onLoggedIn);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::accept()
{
    QEventLoop loop;
    connect(this,&LoginDialog::terminateLoop,&loop,&QEventLoop::quit);
    if(m_request->state()==QTcpSocket::UnconnectedState)
        if(!m_request->tryConnect())
        {
            QMessageBox::critical(this,"Подлкючение к серверу","Не удалось подключиться к серверу.");
            return;
        }
    m_request->login(ui->txtLogin->text(),ui->txtPassword->text());
    loop.exec();
    if(m_result)
        QDialog::accept();
    else
    {
        QMessageBox::warning(this,"Вход в систему","Не удалось войти в систему. Проверьте правильность ввода логина и пароля.");
        //QDialog::reject();
    }
}

void LoginDialog::onLoggedIn(bool success)
{
    m_result=success;
    emit terminateLoop();
}
