//infocollector.h
#ifndef INFOCOLLECTOR_H
#define INFOCOLLECTOR_H
#include "ui_mainwindow.h"
#include "windows.h"
#include "stdlib.h"
#include "stdio.h"
#include <fstream>
#include <string>
#include <sstream>
#include "utility.h"
#include <tlhelp32.h>
#include <assert.h>
#include <lm.h>
#define INFO_ROW_COUNT 15
using namespace std;
class InfoCollector
{
public:
    Ui::MainWindow * gui;
    QString info_strings[2][INFO_ROW_COUNT];
    vector<string> user_list;
    vector<string> process_list;
    vector<string> process_id_list;
    vector< vector<string> > disks_list;
    vector< vector<string> > listOfSoftware;
    InfoCollector(){}
    void collect()
    {
        getcpuinfo();
        getmemoryinfo();
        getosinfo();
        GetProcessList();
        getUsersListInfo();
        getDisksInfo();
        getSoftwareList();
    }
    void saveToFile(LPCWSTR path);
private slots:
    void on_tableWidget_2_clicked(const QModelIndex &index);

private:
    void GetProcessList();
    void getDisksInfo();
    void getosinfo();
    void getmemoryinfo();
    void getcpuinfo();
    void getUsersListInfo();
    void getSoftwareList();
};
#endif // INFOCOLLECTOR_H
