#include "mainwindow.h"
#include "addlinkdialog.h"
#include "logindialog.h"
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>

#include <clientsideproxy.h>
#include <requestbuilder.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientSideProxy * proxy=ClientSideProxy::getInstance();
    RequestBuilder * request_builder=new RequestBuilder(&a);
    if(proxy)
        proxy->setRequestBuilder(request_builder);
    else
        return 0;
    MainWindow w(request_builder);
    w.show();
    w.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            w.size(),
            qApp->desktop()->availableGeometry()
        ));
    LoginDialog logindialog(request_builder);
    logindialog.setWindowTitle("Войти в систему");
    int code=logindialog.exec();
    if(code==QDialog::Rejected)
    {
        w.close();
        qApp->quit();
        return 0;
    }
    else
    {
        w.loadData();
    }

    /*AddLinkDialog links;
    links.setWindowTitle("Добавить ссылку");
    links.show();*/


    return a.exec();
}
