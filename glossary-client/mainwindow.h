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
#include <QTextDocumentFragment>

#include <common.h>
#include <requestbuilder.h>
#include <termindexmodel.h>

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
    void showTerm(TermInfo & ti);
    void genChoisePage(TermInfo & ti);
    ~MainWindow();

public slots:
    void onLinkClicked(QString uri);
private slots:
    void on_termsList_entered(const QModelIndex &index);

    void on_termsList_pressed(const QModelIndex &index);

    void on_termsList_activated(const QModelIndex &index);

    void on_txtKeywordSearch_returnPressed();

    void on_resultsList_activated(const QModelIndex &index);

    void on_actionBack_triggered();

    void on_actionNext_triggered();

    void on_actionRequest_triggered();

    void on_actionAddTerm_triggered();

    void on_actionEdit_triggered();

    void on_actionLogout_triggered();

    void on_actionRefresh_triggered();

private:
    void loadTree();
    void loadIndex();
    void initUI();
    void connectSignals();
    void clearAll();

    TextEdit * textEdit;
    RequestBuilder * m_request;

    QMap<quint32, QTreeWidgetItem *> m_domains;
    QMap<quint32, DomainInfo> m_domains_info;
    QMap<quint32, TermInfo> m_search_result;

    //QTcpSocket * m_socket;
};

#include <main.h>

#endif // MAINWINDOW_H
