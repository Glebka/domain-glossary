#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
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
