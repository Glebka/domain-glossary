#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QList>
#include <textedit.h>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QMap>
#include <QTreeWidgetItem>

#include <common.h>
#include <requestbuilder.h>

enum ItemType {Root,Domain,Term};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    explicit MainWindow(RequestBuilder * builder,QWidget *parent = 0);
    void loadData();
    void loadTree();
    void showTerm(TermInfo & ti);
    void genChoisePage(TermInfo & ti);
    ~MainWindow();

public slots:
    void onLinkClicked(QString uri);
private:

    void initUI();
    void connectSignals();

    TextEdit * textEdit;
    RequestBuilder * m_request;

    QMap<quint32, QTreeWidgetItem *> m_domains;
    QMap<quint32, DomainInfo> m_domains_info;

    //QTcpSocket * m_socket;
};

#endif // MAINWINDOW_H
