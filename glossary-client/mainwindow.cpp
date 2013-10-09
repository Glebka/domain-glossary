#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
    textEdit=new TextEdit(ui->rtfEdit,ui->toolbarArea,this);
    ui->rtfView->append("<a href='http://google.com/'>Some link</a>");
    ui->rtfEdit->append("<a href='http://google.com/'>Some link</a>");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLinkClicked(QString uri)
{
    QDesktopServices::openUrl(QUrl(uri));
}

void MainWindow::initUI()
{
    this->setWindowTitle("Электронный глоссарий");
    ui->stackedWidget->setCurrentIndex(1);

    ui->rtfView->setReadOnly(true);
    connect(ui->rtfView,&TextEditor::linkClicked,this,&MainWindow::onLinkClicked);
    connect(ui->rtfEdit,&TextEditor::linkClicked,this,&MainWindow::onLinkClicked);

    // main layout
    QVBoxLayout * mainLayout=new QVBoxLayout();
    mainLayout->addWidget(ui->splitter);
    mainLayout->setMargin(1);
    ui->centralWidget->setLayout(mainLayout);

    //splitter configuration
    ui->splitter->setStretchFactor(1,1);

    //contents tab layout
    QVBoxLayout * contentsLayout=new QVBoxLayout();
    contentsLayout->addWidget(ui->contentsTree);
    contentsLayout->setMargin(1);
    ui->contentsTab->setLayout(contentsLayout);

    // index tab layout
    ui->indexTab->setLayout(ui->indexLayout);

    // search tab
    ui->searchTab->setLayout(ui->searchLayout);

    // view panel
    QVBoxLayout * viewLayout=new QVBoxLayout();
    viewLayout->addWidget(ui->rtfView);
    viewLayout->setMargin(1);
    ui->viewPage->setLayout(viewLayout);

    //edit panel
    ui->editPage->setLayout(ui->editLayout);
}
