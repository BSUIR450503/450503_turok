#include "infocollector.h"
#include <QString>
#include <tlhelp32.h>
void InfoCollector::getosinfo(){
        long unsigned int size_ = 255;
        LPDWORD size = &size_;
        WCHAR info[256];
        char info_str[256];
        GetComputerNameW(info,size);
        info_strings[0][5] = "Имя компьютера";
        wcstombs(info_str,info,255);
        QString rus_str;
        rus_str = QString::fromWCharArray(info);
        info_strings[1][5] = rus_str.toStdString();

        OSVERSIONINFOEX osversion;
        ZeroMemory(&osversion, sizeof(OSVERSIONINFOEX));
        osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO *) &osversion);
        info_strings[0][6] = "Версия windows";
        info_strings[1][6] = getOsVersionString(osversion.dwMajorVersion,osversion.dwMinorVersion,
                                                (osversion.wProductType == VER_NT_WORKSTATION));
    }


void InfoCollector::getmemoryinfo() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof (status);
    GlobalMemoryStatusEx(&status);
    char buffer[256];
    char * memvolume = new char[100];
    memvolume = itoa(status.ullTotalPhys/1048576,buffer,10);
    info_strings[0][2] = "ОЗУ (Мб)";
    info_strings[1][2] = memvolume;
    memvolume = itoa(status.ullAvailPhys/1048576,buffer,10);
    info_strings[0][3] = "ОЗУ свободно (Мб)";
    info_strings[1][3] = memvolume;
    memvolume = itoa(status.ullTotalPageFile/1048576,buffer,10);
    info_strings[0][4] = "подкачка (swap)";
    info_strings[1][4] = memvolume;
    memvolume = itoa(status.ullAvailPageFile/1048576,buffer,10);
    info_strings[0][5] = "подкачка доступно";
    info_strings[1][5] = memvolume;
}

void InfoCollector::getcpuinfo(){
    TCHAR cpuinfo[256];
    char cpuinfoout[256];
    LPCWSTR path = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    LPCWSTR value= L"ProcessorNameString";
    DWORD dwType = REG_SZ;
    DWORD valueLength = 256;
    HKEY cpuinfokey = 0;
    RegOpenKey(HKEY_LOCAL_MACHINE,path,&cpuinfokey);
    RegQueryValueEx(cpuinfokey,value,NULL,&dwType,reinterpret_cast<LPBYTE>(&cpuinfo),&valueLength);
    wcstombs(cpuinfoout, cpuinfo, 256);
    info_strings[0][0] = "Процессор";
    info_strings[1][0] = cpuinfoout;
    value= L"~MHz";
    dwType = REG_DWORD;
    DWORD frequency;
    valueLength=sizeof(DWORD);
    RegQueryValueEx(cpuinfokey,value,NULL,&dwType,reinterpret_cast<LPBYTE>(&frequency),&valueLength);
    char * freqinfo=itoa(frequency, cpuinfoout,10);
    info_strings[0][1] = "Частота ЦП (МГц)";
    info_strings[1][1] = freqinfo;
}

BOOL InfoCollector::GetProcessList( )
{
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;

    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    puts( "error: CreateToolhelp32Snapshot (of processes)");
    return( FALSE );
  }

  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );

  // exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );
    return( FALSE );
  }


  int row=0;
  do
  {
      gui->tableWidget_2->insertRow(row);
      gui->tableWidget_2->setItem(row,0,new QTableWidgetItem());
      gui->tableWidget_2->setItem(row,1,new QTableWidgetItem());
      QString wstr;
      char buf[30];
      wstr = QString::fromWCharArray(pe32.szExeFile);
      gui->tableWidget_2->item(row,1)->setText(itoa(pe32.th32ProcessID,buf,10));
      gui->tableWidget_2->item(row,0)->setText(wstr);
    row++;

  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return( TRUE );
}
