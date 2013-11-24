#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(RequestBuilder *builder, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , m_request(builder)
{
    ui->setupUi(this);
    initUI();
    textEdit=new TextEdit(ui->rtfEdit,ui->toolbarArea,this);
    ui->rtfView->append("<a href='http://google.com/'>Some link</a>");
    ui->rtfEdit->append("<a href='http://google.com/'>Some link</a>");
    foreach (QAction * act, ui->mainToolBar->actions()) {
        act->setEnabled(false);
    }
    /*m_socket=new QTcpSocket(this);
    m_socket->connectToHost(QHostAddress("127.0.0.1"),1111);
    QTcpSocket * socket=m_socket;
    connect(m_socket,&QTcpSocket::connected,[socket](){
        QDataStream stream(socket);
        stream<<CMD_LOGIN;
        stream<<QString("rgewebppc@gmail.com");
        stream<<QString("12");
    });
    connect(m_socket,&QTcpSocket::readyRead,[socket](){
        QDataStream stream(socket);
        quint32 code;
        stream>>code;
        if(code==CMD_OK)
        {
            qDebug()<<"Login OK!";
            UserInfo ui;
            stream>>ui;
            qDebug()<<ui.full_name;
        }
        else
            qDebug()<<"Error on login!";
    });*/
}

void MainWindow::loadData()
{
    loadTree();
}

void MainWindow::loadTree()
{
    QTreeWidgetItem * topLevel=new QTreeWidgetItem(ui->contentsTree,0);
    topLevel->setText(0,"Глоссарий");
    ui->contentsTree->addTopLevelItem(topLevel);
    topLevel->setData(1,Qt::UserRole,QVariant(Root));
    topLevel->addChild(new QTreeWidgetItem());

    connect(ui->contentsTree,&QTreeWidget::itemCollapsed,[](QTreeWidgetItem * item){
        QTreeWidgetItem * it=item;
        qDeleteAll(it->takeChildren());
        it->addChild(new QTreeWidgetItem());
    });
    RequestBuilder * request_builder=m_request;
    connect(ui->contentsTree,&QTreeWidget::itemExpanded,[request_builder](QTreeWidgetItem * item){
        QTreeWidgetItem * it=item;
        qDeleteAll(it->takeChildren());
        switch(it->data(1,Qt::UserRole).toInt())
        {
        case Root:
            request_builder->getAllDomains();
            break;
        case Domain:
            request_builder->getAllTermsByDomain(it->data(0,Qt::UserRole).toUInt());
            break;
        default:
            break;
        }
    });

    connect(m_request,&RequestBuilder::loadDomains,[this,topLevel](QList<DomainInfo> domains){
        qDeleteAll(topLevel->takeChildren());
        foreach (DomainInfo di, domains) {
            QTreeWidgetItem * item=new QTreeWidgetItem(topLevel);
            item->setText(0,di.title);
            item->addChild(new QTreeWidgetItem());
            item->setData(0,Qt::UserRole,QVariant(di.id));
            item->setData(1,Qt::UserRole,QVariant(Domain));
            this->m_domains[di.id]=item;
            this->m_domains_info[di.id]=di;
        }
    });
    connect(m_request,&RequestBuilder::loadTermsByDomain,[this](QList<TermInfo> terms){
        if(terms.size()==0) return;
        quint32 domainId=terms.first().domain_id;
        if(!this->m_domains.contains(domainId))
            return;
        QTreeWidgetItem * parent=this->m_domains[domainId];
        foreach (TermInfo ti, terms) {
            QTreeWidgetItem * item=new QTreeWidgetItem(parent);
            item->setText(0,ti.title);
            item->setData(0,Qt::UserRole,QVariant(ti.id));
            item->setData(1,Qt::UserRole,QVariant(Term));
            QByteArray data;
            QDataStream stream(&data,QIODevice::WriteOnly);
            stream<<ti;
            item->setData(2,Qt::UserRole,QVariant(data));
        }
    });
    connect(ui->contentsTree,&QTreeWidget::itemSelectionChanged,[this](){
        QList<QTreeWidgetItem *> items=this->ui->contentsTree->selectedItems();
        if(items.size()==0) return;
        QTreeWidgetItem * item=items.first();
        if(item->data(1,Qt::UserRole).toInt()!=Term)
            return;
        QByteArray data=item->data(2,Qt::UserRole).toByteArray();
        QDataStream stream(&data,QIODevice::ReadOnly);
        TermInfo ti;
        stream>>ti;
        showTerm(ti);
    });
}

void MainWindow::showTerm(TermInfo &ti)
{
    if(ti.concept_list.size()>1)
        genChoisePage(ti);
}

void MainWindow::genChoisePage(TermInfo &ti)
{
    QTextDocument * document=ui->rtfView->document();
    document->clear();
    QTextCursor cursor=ui->rtfView->textCursor();
    QStringList html;
    html<<"<h1>"<<ti.title<<"</h1><hr/><br>";
    cursor.insertHtml(html.join(""));
    cursor.insertImage(QImage(":/icons/multiple-values.png"));
    cursor.insertHtml("<p><i>Этот термин имеет несколько значений.</i></p><br>");
    /*cursor.insertHtml("<ol>");
    QTextCursor * cur=new QTextCursor(cursor);
    cursor.insertHtml("</ol>");
    foreach (quint32 ci, ti.concept_list) {
        m_request->getConcept(ci);
    }
    connect(m_request,&RequestBuilder::loadConcept,[this,cur,ti](ConceptInfo ci){
        cur->insertHtml("<li><a href='term:"+QString::number(ci.term_list.first())
                        +":"+QString::number(ci.id)+"'>"+ti.title+" ("+this->m_domains_info[ci.domain_id].title.toLower()
                        +")</a></li>");
    });*/
}

void MainWindow::connectSignals()
{

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
    ui->stackedWidget->setCurrentIndex(0);

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
