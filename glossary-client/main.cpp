#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QThread>

#include "mainwindow.h"
#include "logindialog.h"

#include <clientsideproxy.h>
#include <requestbuilder.h>

#include <main.h>

//NESS, Erick - Stereoceltic

void login(RequestBuilder * request_builder,MainWindow * w)
{
    LoginDialog logindialog(request_builder);
    logindialog.setWindowTitle("Войти в систему");
    int code=logindialog.exec();
    if(code==QDialog::Rejected)
    {
        w->close();
        qApp->quit();
        exit(0);
    }
    else
        w->loadData();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientSideProxy * proxy=ClientSideProxy::getInstance();
    RequestBuilder * request_builder=new RequestBuilder();
    QThread * thread=new QThread();
    request_builder->moveToThread(thread);

    a.connect(thread,&QThread::finished,request_builder,&RequestBuilder::deleteLater);
    a.connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    a.connect(&a,&QApplication::lastWindowClosed,thread,&QThread::quit);

    thread->start();

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
    login(request_builder,&w);
    /*AddLinkDialog links;
    links.setWindowTitle("Добавить ссылку");
    links.show();*/


    return a.exec();
}
