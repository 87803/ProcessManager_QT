#include "mainwindow.h"


void setTable(QStandardItemModel *model, QTableView* qtv)
{
    model->setColumnCount(7);
    model->setHeaderData(0,Qt::Horizontal,"名称");
    model->setHeaderData(1,Qt::Horizontal,"PID");
    model->setHeaderData(2,Qt::Horizontal,"优先级");
    model->setHeaderData(3,Qt::Horizontal,"需要时间");
    model->setHeaderData(4,Qt::Horizontal,"运行时间");
    model->setHeaderData(5,Qt::Horizontal,"内存大小");
    model->setHeaderData(6,Qt::Horizontal,"页表(分配的物理块)");
    qtv->verticalHeader()->hide();
    qtv->setModel(model);
    qtv->setColumnWidth(0,82);
    qtv->setColumnWidth(1,40);
    qtv->setColumnWidth(2,45);
    qtv->setColumnWidth(3,55);
    qtv->setColumnWidth(4,55);
    qtv->setColumnWidth(5,55);
    qtv->setColumnWidth(6,118);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model1 = new QStandardItemModel();
    model2 = new QStandardItemModel();
    model3 = new QStandardItemModel();
    model4 = new QStandardItemModel();
    setTable(model1,ui->runningTableView);
    setTable(model2,ui->blockTableView);
    setTable(model3,ui->readyTableView);
    setTable(model4,ui->createTableView);
    ui->systemInfo->setText("系统信息：\n总内存：32,768KB\n页面大小：1024KB\n已分配的物理块："+
                            QString::number(32-procManager.getPageNum())+"/32\n内存利用率："+
                            QString::number((double)(32-procManager.getPageNum())/0.32)+"%\n"+
                               "运行中的进程数："+QString::number(procManager.getRunningProcNum())+"/4");
    /*ui->runningTableWidget->setColumnWidth(0,82);
    ui->runningTableWidget->setColumnWidth(1,40);
    ui->runningTableWidget->setColumnWidth(2,45);
    ui->runningTableWidget->setColumnWidth(3,55);
    ui->runningTableWidget->setColumnWidth(4,55);
    ui->runningTableWidget->setColumnWidth(5,55);
    ui->runningTableWidget->setColumnWidth(6,118);

    ui->readyTableWidget->setColumnWidth(0,82);
    ui->readyTableWidget->setColumnWidth(1,40);
    ui->readyTableWidget->setColumnWidth(2,45);
    ui->readyTableWidget->setColumnWidth(3,55);
    ui->readyTableWidget->setColumnWidth(4,55);
    ui->readyTableWidget->setColumnWidth(5,55);
    ui->readyTableWidget->setColumnWidth(6,118);

    ui->createTableWidget->setColumnWidth(0,82);
    ui->createTableWidget->setColumnWidth(1,40);
    ui->createTableWidget->setColumnWidth(2,45);
    ui->createTableWidget->setColumnWidth(3,55);
    ui->createTableWidget->setColumnWidth(4,55);
    ui->createTableWidget->setColumnWidth(5,55);
    ui->createTableWidget->setColumnWidth(6,118);

    ui->blockTableWidget->setColumnWidth(0,82);
    ui->blockTableWidget->setColumnWidth(1,40);
    ui->blockTableWidget->setColumnWidth(2,45);
    ui->blockTableWidget->setColumnWidth(3,55);
    ui->blockTableWidget->setColumnWidth(4,55);
    ui->blockTableWidget->setColumnWidth(5,55);
    ui->blockTableWidget->setColumnWidth(6,118);

    ui->runningTableWidget->verticalHeader()->hide();
    ui->runningTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->blockTableWidget->verticalHeader()->hide();
    ui->blockTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->createTableWidget->verticalHeader()->hide();
    ui->createTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->readyTableWidget->verticalHeader()->hide();
    ui->readyTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_submitButton_clicked()
{
    procManager.create(ui->procName->text().toStdString(),
                       ui->priority->value(),ui->needTime->value(),ui->memory->value());
    updateTableView();
}

void MainWindow::updateTableView()
{
    ui->systemInfo->setText("系统信息：\n总内存：32,768KB\n页面大小：1024KB\n已分配的物理块："+
                            QString::number(32-procManager.getPageNum())+"/32\n内存利用率："+
                            QString::number((double)(32-procManager.getPageNum())/0.32)+"%\n"+
                               "运行中的进程数："+QString::number(procManager.getRunningProcNum())+"/4");
    ui->textBrowser->setText(QString::fromStdString(procManager.message));
    updateTableView(procManager.getRunningList(), model1);
    updateTableView(procManager.getBlockList(), model2);
    updateTableView(procManager.getReadyList(), model3);
    updateTableView(procManager.getCreateList(), model4);

}

void MainWindow::updateTableView(const PCB* temp, QStandardItemModel* qtv)
{
    qtv->removeRows(0,qtv->rowCount());
    int i=0;
    while(temp)
    {
        qtv->setItem(i,0,new QStandardItem(QString::fromStdString(temp->name)));
        qtv->setItem(i,1,new QStandardItem(QString::number(temp->id)));
        qtv->setItem(i,2,new QStandardItem(QString::number(temp->priority)));
        qtv->setItem(i,3,new QStandardItem(QString::number(temp->needTime)));
        qtv->setItem(i,4,new QStandardItem(QString::number(temp->runTime)));
        qtv->setItem(i,5,new QStandardItem(QString::number(temp->memorySize, 'f', 2)));
        if(qtv!=model4)
        {
            QString pageItem="["+ QString::number(temp->pt->address)+"]: ";
            for(int j=0;j<_msize(temp->pt->tableItem) / sizeof(temp->pt->tableItem[0]);j++)
                pageItem+=(QString::number(temp->pt->tableItem[j])+" ");
            qtv->setItem(i,6,new QStandardItem(pageItem));
        }
        else
            qtv->setItem(i,6,new QStandardItem("NULL"));
        i++;
        temp=temp->next;
    }
}


void MainWindow::on_blockButton_clicked()
{
    procManager.block(ui->pid->value());
    updateTableView();
}


void MainWindow::on_awakeButton_clicked()
{
    procManager.wakeup(ui->pid->value());
    updateTableView();
}


void MainWindow::on_killButton_clicked()
{
    procManager.kill(ui->pid->value());
    updateTableView();
}


void MainWindow::on_runButton_clicked()
{
    procManager.procRunning();
    updateTableView();
}

