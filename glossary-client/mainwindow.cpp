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
    //ui->rtfView->append("<a href='http://google.com/'>Some link</a>");
    //ui->rtfEdit->append("<a href='http://google.com/'>Some link</a>");
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
    qRegisterMetaType<QVector<int>>("QVector<int>");
    loadTree();
    loadIndex();
    foreach (QAction * act, ui->mainToolBar->actions()) {
        act->setEnabled(true);
    }
    ui->actionEdit->setEnabled(false);
    if(m_request->user.type!=Expert)
        ui->actionAddTerm->setEnabled(false);
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
            item->setToolTip(0,di.title);
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
            item->setToolTip(0,ti.title);
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
   //ui->contentsTree->topLevelItem(0)->setExpanded(true);
}

void MainWindow::loadIndex()
{
    QAbstractItemModel * old=ui->termsList->model();
    if(old)
        delete old;
    TermIndexModel * model=new TermIndexModel(m_request,&m_domains_info,this);
    ui->termsList->setModel(model);
}

void MainWindow::showTerm(TermInfo &ti)
{
    ui->stackedWidget->setCurrentIndex(0);
    qDebug()<<"Term #"<<ti.id;
    QTextDocument * document=ui->rtfView->document();
    document->clear();
    QTextCursor cursor=ui->rtfView->textCursor();
    QStringList html;
    html<<"<h1>"<<ti.title<<"</h1><hr/><br>";
    //cursor.movePosition(QTextCursor::Start);
    cursor.insertHtml(html.join(""));

    m_request->startTransaction();
    m_request->getConcept(ti.concept_list.first());
    m_request->getConceptText(ti.concept_list.first());
    QByteArray bytes=m_request->endTransaction();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    QString text;
    ConceptInfo ci;
    stream>>ci;
    stream>>text;
    if(text.length()==0)
    {
        QFile file(":/empty.html");
        if(file.open(QIODevice::ReadOnly))
        {
            text=file.readAll();
        }
    }
    //ui->rtfView->setText(text);
    cursor.insertHtml(text);
    cursor.insertHtml("<br><hr><br><i>Последнее изменение: "+ci.last_modified.toString(DATE_TIME_FORMAT)+"</i>");
}

void MainWindow::startEditTerm(TermInfo ti)
{
    ui->stackedWidget->setCurrentIndex(1);
    QTextDocument * document=ui->rtfEdit->document();
    document->clear();
    QTextCursor cursor=ui->rtfEdit->textCursor();
    m_request->startTransaction();
    m_request->getDomainById(ti.domain_id);
    m_request->getConcept(ti.concept_list.first());
    m_request->getConceptText(ti.concept_list.first());
    QByteArray bytes=m_request->endTransaction();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    QString text;
    ConceptInfo ci;
    DomainInfo di;
    stream>>di;
    stream>>ci;
    stream>>text;
    ui->txtDomain->setText(di.title);
    ui->txtTerm->setText(ti.title);
    ui->txtKeywords->setText(ci.keywords.join(", "));
    cursor.insertHtml(text);
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
    m_request->startTransaction();
    html.clear();
    html<<"<ul>";
    qDebug()<<ti.concept_list;
    foreach (quint32 ci, ti.concept_list) {
        m_request->getConcept(ci);
    }
    QByteArray bytes=m_request->endTransaction();
    QDataStream stream(&bytes,QIODevice::ReadOnly);
    while(!stream.atEnd())
    {
        ConceptInfo ci;
        stream>>ci;
        html<<"<li><a href='#term:"<<QString::number(ci.term_list.first())
           <<":"<<QString::number(ci.id)<<"'>"<<ti.title<<" ("<<m_domains_info[ci.domain_id].title.toLower()
            <<")</a></li>";
    }
    html<<"</ul>";
    cursor.insertHtml(html.join(""));
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

void MainWindow::clearAll()
{
    m_domains.clear();
    m_domains_info.clear();
    m_search_result.clear();
    ui->contentsTree->clear();
    ui->resultsList->clear();
    ui->rtfView->clear();
    ui->rtfEdit->clear();
    ui->txtKeywordSearch->clear();
    foreach (QAction * act, ui->mainToolBar->actions()) {
        act->setEnabled(false);
    }

    disconnect(ui->contentsTree,&QTreeWidget::itemCollapsed,0,0);
    disconnect(ui->contentsTree,&QTreeWidget::itemExpanded,0,0);
    disconnect(m_request,&RequestBuilder::loadDomains,0,0);
    disconnect(m_request,&RequestBuilder::loadTermsByDomain,0,0);
    disconnect(ui->contentsTree,&QTreeWidget::itemSelectionChanged,0,0);
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

void MainWindow::on_termsList_entered(const QModelIndex &index)
{

}

void MainWindow::on_termsList_pressed(const QModelIndex &index)
{

}

void MainWindow::on_termsList_activated(const QModelIndex &index)
{
    // bug with transactions
    TermInfo ti=*(TermInfo *)index.internalPointer();
    showTerm(ti);
}

void MainWindow::on_txtKeywordSearch_returnPressed()
{
    ui->resultsList->clear();
    m_search_result.clear();
    QString search=ui->txtKeywordSearch->text();
    m_request->startTransaction();
    m_request->search(search);
    QByteArray array=m_request->endTransaction();
    QDataStream stream(&array,QIODevice::ReadOnly);
    QList<quint32> term_ids;
    stream>>term_ids;
    if(term_ids.size()==0)
    {
        QMessageBox::information(this,"Поиск","Поиск не дал результатов.");
        return;
    }
    m_request->startTransaction();
    foreach (quint32 term_id, term_ids) {
        m_request->getTerm(term_id);
    }
    QByteArray terms_data=m_request->endTransaction();
    QDataStream terms_stream(&terms_data,QIODevice::ReadOnly);
    TermInfo ti;
    TermInfo * term_ptr=0;
    while(!terms_stream.atEnd())
    {
        terms_stream>>ti;
        m_search_result[ti.id]=ti;
        term_ptr=&m_search_result[ti.id];
        QListWidgetItem * item=new QListWidgetItem(ti.title);
        item->setData(Qt::UserRole,QVariant((qint64)term_ptr));
        ui->resultsList->addItem(item);
    }
}

void MainWindow::on_resultsList_activated(const QModelIndex &index)
{
    if(index.isValid())
    {
        QListWidgetItem * item=(QListWidgetItem *)index.internalPointer();
        TermInfo * ti=(TermInfo *)item->data(Qt::UserRole).toLongLong();
        showTerm(*ti);
    }
}

void MainWindow::on_actionBack_triggered()
{

}

void MainWindow::on_actionNext_triggered()
{

}

void MainWindow::on_actionRequest_triggered()
{

}

void MainWindow::on_actionAddTerm_triggered()
{
    AddTermDialog dialog(m_request,ui->termsList->model(),&m_domains_info,this);
    if(dialog.exec()==QDialog::Accepted)
    {
        loadIndex();
        TermInfo ti=dialog.dialogResult();
        if(m_domains.contains(ti.domain_id))
        {
            QTreeWidgetItem * parent=m_domains[ti.domain_id];
            if(parent->isExpanded())
            {
                QTreeWidgetItem * item=new QTreeWidgetItem(parent);
                item->setText(0,ti.title);
                item->setData(0,Qt::UserRole,QVariant(ti.id));
                item->setData(1,Qt::UserRole,QVariant(Term));
                item->setToolTip(0,ti.title);
                QByteArray data;
                QDataStream stream(&data,QIODevice::WriteOnly);
                stream<<ti;
                item->setData(2,Qt::UserRole,QVariant(data));
            }
        }
        if(dialog.isAutoStartEdit())
            startEditTerm(ti);
    }
}

void MainWindow::on_actionEdit_triggered()
{

}

void MainWindow::on_actionLogout_triggered()
{
    clearAll();
    m_request->disconnectFromHost();
    qDebug()<<"--------------------------------------";
    login(m_request,this);
}

void MainWindow::on_actionRefresh_triggered()
{
    clearAll();
    loadData();
}
