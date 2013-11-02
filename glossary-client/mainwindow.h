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

#include <common.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onLinkClicked(QString uri);
private:
    void initUI();
    TextEdit * textEdit;
    QTcpSocket * m_socket;
};

#endif // MAINWINDOW_H
