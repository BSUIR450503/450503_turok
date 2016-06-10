#include "mainwindow.h"
#include "windows.h"
#include "ui_mainwindow.h"
#include "infocollector.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->statusBar()->setSizeGripEnabled(false);
    this->setFixedSize(size());
    info_obj.gui=ui;
    ui->tableWidget->setColumnWidth(0,150);
    ui->tableWidget->setColumnWidth(1,300);
    ui->tableWidget->setRowCount(INFO_ROW_COUNT);
    ui->tableWidget_2->setColumnWidth(0,500);
    ui->tableWidget_2->setColumnWidth(1,50);
    for (int i=0;i<INFO_ROW_COUNT;i++)
        for (int j=0;j<2;j++) {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem());
            ui->tableWidget->setItem(i,j,new QTableWidgetItem());
        }
    info_obj.collect();
    for (int i=0;i<2;i++)
        for (int j=0;j<INFO_ROW_COUNT;j++) {
            if (info_obj.info_strings[i][j].isEmpty()) continue;
            ui->tableWidget->item(j,i)->setText(info_obj.info_strings[i][j]);
        }
    ui->users_table->insertColumn(0);
    ui->users_table->setColumnWidth(0,600);

   for (unsigned int i=0;i<info_obj.process_list.size();i++){

        ui->tableWidget_2->insertRow(i);
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem());
        ui->tableWidget_2->item(i,0)->setText(info_obj.process_list[i].c_str());
        ui->tableWidget_2->setItem(i,1,new QTableWidgetItem());
        ui->tableWidget_2->item(i,1)->setText(info_obj.process_id_list[i].c_str());
    }

    for (unsigned int i=0;i<info_obj.user_list.size();i++){

        ui->users_table->insertRow(i);
        ui->users_table->setItem(i,0,new QTableWidgetItem());
        ui->users_table->item(i,0)->setText(info_obj.user_list[i].c_str());
    }
    ui->tableWidget_2->horizontalHeader()->setVisible(true);
    ui->tableWidget_3->setColumnWidth(3,150);
    for (unsigned int i=0;i<info_obj.disks_list.size();i++){
         ui->tableWidget_3->insertRow(i);
         for (int j=0;j<5;j++) {
         ui->tableWidget_3->setItem(i,j,new QTableWidgetItem());
         ui->tableWidget_3->item(i,j)->setText(info_obj.disks_list[i][j].c_str());
         }
     }
    ui->tableWidget_4->setColumnWidth(0,400);
    ui->tableWidget_4->setColumnWidth(1,150);
    for (unsigned int i=0;i<info_obj.listOfSoftware.size();i++){
         ui->tableWidget_4->insertRow(i);
         for (int j=0;j<2;j++) {
         ui->tableWidget_4->setItem(i,j,new QTableWidgetItem());
         ui->tableWidget_4->item(i,j)->setText(info_obj.listOfSoftware[i][j].c_str());
         }
     }
    //ui->tableWidget->repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    WCHAR fileNameBuf[MAX_PATH] = L"";
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = (LPCWSTR)L".txt";
    ofn.lpstrFile = fileNameBuf;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = (LPCWSTR)L"txt";
    if(!GetSaveFileNameW(&ofn)) return;
    info_obj.saveToFile(ofn.lpstrFile);
}
