#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "infocollector.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    info_obj.gui=ui;
    ui->tableWidget->setColumnWidth(0,130);
    ui->tableWidget->setColumnWidth(1,300);
    ui->tableWidget_2->setColumnWidth(0,500);
    ui->tableWidget_2->setColumnWidth(1,100);
    for (int i=0;i<10;i++)
        for (int j=0;j<2;j++) {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem());
            ui->tableWidget->setItem(i,j,new QTableWidgetItem());
        }
   /* gui->tableWidget_2->insertRow(row);
    gui->tableWidget_2->setItem(row,0,new QTableWidgetItem());
    gui->tableWidget_2->setItem(row,1,new QTableWidgetItem());
    gui->tableWidget_2->item(row,0)->setText("Процесс:");
    gui->tableWidget_2->item(row,1)->setText("ID");*/
    info_obj.collect();
    for (int i=0;i<2;i++)
        for (int j=0;j<10;j++) {
            if (info_obj.info_strings[i][j].empty()) continue;
            ui->tableWidget->item(j,i)->setText(info_obj.info_strings[i][j].c_str());
        }
    ui->tableWidget->repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}
