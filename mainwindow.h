#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <QTableView>
#include <QStandardItemModel>
#include <QGuiApplication>
#include "ui_mainwindow.h"
#include "ProcessManager.h"
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_submitButton_clicked();
    void on_blockButton_clicked();
    void on_awakeButton_clicked();
    void on_killButton_clicked();
    void on_runButton_clicked();

private:
    Ui::MainWindow *ui;
    ProcessManager procManager;
    QStandardItemModel *model1;
    QStandardItemModel *model2;
    QStandardItemModel *model3;
    QStandardItemModel *model4;
    void updateTableView();
    void updateTableView(const PCB* temp, QStandardItemModel* qtv);
};
#endif // MAINWINDOW_H
