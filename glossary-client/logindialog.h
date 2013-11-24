#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QEventLoop>
#include <QMessageBox>

#include <requestbuilder.h>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(RequestBuilder * builder,QWidget *parent = 0);
    ~LoginDialog();
    void accept();

signals:
    void terminateLoop();
public slots:
    void onLoggedIn(bool success);
private:
    Ui::LoginDialog *ui;
    RequestBuilder * m_request;
    bool m_result;
};

#endif // LOGINDIALOG_H
