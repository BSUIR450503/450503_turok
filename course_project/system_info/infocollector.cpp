#include "infocollector.h"
#include <QString>
//#include <wmiutils.h>
//#include <wmiatlprov.h>
//#include <wmistr.h>
#pragma comment(lib, "netapi32.lib")
void InfoCollector::getosinfo(){
        long unsigned int size_ = 255;
        LPDWORD size = &size_;
        WCHAR info[256];
        GetComputerNameW(info,size);
        info_strings[0][7] = "Имя компьютера";
        QString rus_str;
        rus_str = QString::fromWCharArray(info);
        info_strings[1][7] = rus_str.toStdString();

        OSVERSIONINFOEX osversion;
        ZeroMemory(&osversion, sizeof(OSVERSIONINFOEX));
        osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO *) &osversion);
        info_strings[0][8] = "Версия windows";
        info_strings[1][8] = getOsVersionString(osversion.dwMajorVersion,osversion.dwMinorVersion,
                                                (osversion.wProductType == VER_NT_WORKSTATION));
        info_strings[0][9] = "Текущий пользователь:";
        GetUserNameW(info,size);
        rus_str = QString::fromWCharArray(info);
        info_strings[1][9] = rus_str.toStdString();

        LPWSTR	lpNameBuffer;
        NET_API_STATUS nas;
        NETSETUP_JOIN_STATUS BufferType;
        nas = NetGetJoinInformation(NULL, &lpNameBuffer, &BufferType);
        if (nas != NERR_Success) return;
        switch (BufferType)
        {
        case NetSetupUnknownStatus:
            rus_str = "неизвестно";
            break;
        case NetSetupUnjoined:
            rus_str = "не присоединен";
            break;

        case NetSetupWorkgroupName:
            rus_str = QString::fromWCharArray(lpNameBuffer);
            break;

        case NetSetupDomainName:
            rus_str = QString::fromWCharArray(lpNameBuffer);
            break;
        }
        NetApiBufferFree(lpNameBuffer);
        info_strings[0][10] = "Раб. группа / домен";
        info_strings[1][10] = rus_str.toStdString();
        DISPLAY_DEVICE DevInfo;
        DevInfo.cb = sizeof(DISPLAY_DEVICE);
        EnumDisplayDevices (NULL, 0, &DevInfo, 0);
        rus_str = QString::fromWCharArray(DevInfo.DeviceString);
        info_strings[0][11] = "Видеоадаптер";
        info_strings[1][11] = rus_str.toStdString();
        LCID locale;


    }


void InfoCollector::getmemoryinfo() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof (status);
    GlobalMemoryStatusEx(&status);
    char buffer[256];
    char * memvolume = new char[100];
    memvolume = itoa(status.ullTotalPhys/1048576,buffer,10);
    info_strings[0][3] = "ОЗУ (Мб)";
    info_strings[1][3] = memvolume;
    memvolume = itoa(status.ullAvailPhys/1048576,buffer,10);
    info_strings[0][4] = "ОЗУ свободно (Мб)";
    info_strings[1][4] = memvolume;
    memvolume = itoa(status.ullTotalPageFile/1048576,buffer,10);
    info_strings[0][5] = "подкачка (swap)";
    info_strings[1][5] = memvolume;
    memvolume = itoa(status.ullAvailPageFile/1048576,buffer,10);
    info_strings[0][6] = "подкачка доступно";
    info_strings[1][6] = memvolume;
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

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    char * cores = itoa(sysinfo.dwNumberOfProcessors,cpuinfoout,10);
    info_strings[0][2] = "Число ядер";
    info_strings[1][2] = cores;
}

BOOL InfoCollector::GetProcessList( )
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;

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


  do
  {
      QString wstr;
      char buf[30];
      wstr = QString::fromWCharArray(pe32.szExeFile);
      process_list.push_back(wstr.toStdString());
      string id = itoa(pe32.th32ProcessID,buf,10);
      process_id_list.push_back(id);
  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return( TRUE );
}

void InfoCollector::getUsersListInfo(){
       LPUSER_INFO_0 pBuf = NULL;
       LPUSER_INFO_0 pTmpBuf;
       DWORD dwLevel = 0;
       DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
       DWORD dwEntriesRead = 0;
       DWORD dwTotalEntries = 0;
       DWORD dwResumeHandle = 0;
       DWORD i;
       DWORD dwTotalCount = 0;
       NET_API_STATUS nStatus;
       LPTSTR pszServerName = NULL;

       do
       {
          nStatus = NetUserEnum((LPCWSTR) pszServerName,
                                dwLevel,
                                FILTER_NORMAL_ACCOUNT, // global users
                                (LPBYTE*)&pBuf,
                                dwPrefMaxLen,
                                &dwEntriesRead,
                                &dwTotalEntries,
                                &dwResumeHandle);
          // If the call succeeds,
          if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
          {
             if ((pTmpBuf = pBuf) != NULL)
             {
                for (i = 0; (i < dwEntriesRead); i++)
                {
                   assert(pTmpBuf != NULL);

                   if (pTmpBuf == NULL)
                   {
                      fprintf(stderr, "An access violation has occurred\n");
                      break;
                   }
                   QString wide_str;
                   wide_str = QString::fromWCharArray(pTmpBuf->usri0_name);
                   user_list.push_back(wide_str.toStdString());
                   pTmpBuf++;
                   dwTotalCount++;
                }
             }
          }
          else
             fprintf(stderr, "A system error has occurred: %d\n", nStatus);
          // Free the allocated buffer.
          if (pBuf != NULL)
          {
             NetApiBufferFree(pBuf);
             pBuf = NULL;
          }
       }
       while (nStatus == ERROR_MORE_DATA);

       if (pBuf != NULL)
          NetApiBufferFree(pBuf);
}

void InfoCollector::getDisksInfo(){
    DWORD cchBuffer;
        WCHAR* driveStrings;
        UINT driveType;
        PWSTR driveTypeString;
        ULARGE_INTEGER freeSpace;

        // Find out how big a buffer we need
        cchBuffer = GetLogicalDriveStrings(0, NULL);

        driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(TCHAR));
        if (driveStrings == NULL)
        {
            return;
        }

        // Fetch all drive strings
        GetLogicalDriveStrings(cchBuffer, driveStrings);
        WCHAR nameBuffer[100];
        WCHAR SysNameBuffer[100];
        DWORD VSNumber;
        DWORD MCLength;
        DWORD FileSF;
        bool isPresent;
        // Loop until we find the final '\0'
        // driveStrings is a double null terminated list of null terminated strings)
        while (*driveStrings)
        {
            // Dump drive information
            driveType = GetDriveType(driveStrings);
            QString type;
            QString space;
            QString letter;
            QString fileSystem;
            QString serialNumber;
            switch (driveType)
            {
            case DRIVE_FIXED:
                driveTypeString = L"HDD";
                break;

            case DRIVE_CDROM:
                driveTypeString = L"CD/DVD";
                break;

            case DRIVE_REMOVABLE:
                driveTypeString = L"Съемный";
                break;

            case DRIVE_REMOTE:
                driveTypeString = L"Сетевой";
                break;
            case DRIVE_RAMDISK:
                driveTypeString = L"RAM";
                break;
            default:
                driveTypeString = L"Неизвестно";
                break;
            }
            isPresent = GetDiskFreeSpaceEx(driveStrings, &freeSpace, NULL, NULL);
            if (!isPresent) {
                type = QString::fromWCharArray(driveTypeString);
                space = "-";
                letter = QString::fromWCharArray(driveStrings);
                fileSystem = "-";
                serialNumber = "-";
            }
            else {
            GetVolumeInformationW(driveStrings,nameBuffer,99,&VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer));

            type = QString::fromWCharArray(driveTypeString);
            space = QString::number(freeSpace.QuadPart / 1024 / 1024 / 1024);
            letter = QString::fromWCharArray(driveStrings);
            fileSystem = QString::fromWCharArray(SysNameBuffer);
            serialNumber = QString::number(VSNumber);
            }
            vector<string> one_disk_info;
            one_disk_info.push_back(letter.toStdString());
            one_disk_info.push_back(type.toStdString());
            one_disk_info.push_back(space.toStdString());
            one_disk_info.push_back(fileSystem.toStdString());
            one_disk_info.push_back(serialNumber.toStdString());
            disks_list.push_back(one_disk_info);
            // Move to next drive string
            // +1 is to move past the null at the end of the string.
            driveStrings += lstrlen(driveStrings) + 1;
        }

        free(driveStrings);

        return;
}
