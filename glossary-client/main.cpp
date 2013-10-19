#include "mainwindow.h"
#include "addlinkdialog.h"
#include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    AddLinkDialog links;
    LoginDialog logindialog;
    links.setWindowTitle("Добавить ссылку");
    logindialog.setWindowTitle("Войти в систему");
    links.show();
    logindialog.show();


    return a.exec();
}
