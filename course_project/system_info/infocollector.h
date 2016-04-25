#ifndef INFOCOLLECTOR_H
#define INFOCOLLECTOR_H
#include "ui_mainwindow.h"
#include "windows.h"
#include "stdlib.h"
#include "stdio.h"
#include <iostream>
#include <string>
#include <sstream>
#include "utility.h"
using namespace std;
class InfoCollector
{
public:
    Ui::MainWindow * gui;
    string info_strings[2][10];
    InfoCollector(){}
    void collect()
    {
        getcpuinfo();
        getmemoryinfo();
        getosinfo();
        GetProcessList();
    }
private:
    BOOL GetProcessList();
    void getosinfo();
    void getmemoryinfo();
    void getcpuinfo();
};
#endif // INFOCOLLECTOR_H
