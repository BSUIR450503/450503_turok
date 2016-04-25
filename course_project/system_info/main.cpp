#include "mainwindow.h"
#include <QApplication>
#include <clocale>
using namespace std;
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"ru_RU.utf8");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
