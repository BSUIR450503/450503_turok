
#ifdef _WIN32
#include "stdafx.h"
#include "windows.h"
#include <string>
#include <iostream>
#include "snprintf.h"

using namespace std;
int waitforprint(char * number)
{
	while(1)
	{
		string eventname;
		string evnumber;
		evnumber = number;
		eventname = evnumber + "noprint";
		wchar_t * t=new wchar_t[256];
		mbstowcs(t,eventname.c_str(),128);
		fflush(stdout);
		HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,t);
		if(WaitForSingleObject(printevent,10) == WAIT_OBJECT_0)
		{
			return -1;
		}
		string printeventname = evnumber + "print";
		wchar_t * tt=new wchar_t[256];
		mbstowcs(tt,printeventname.c_str(),256);
		printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,tt);
		if(WaitForSingleObject(printevent,10) == WAIT_OBJECT_0)
		{   
			return 1;
		}
		Sleep(10);
		return 0;
	}
}
void usleep(int s)
{
	s/=1000;
	Sleep(s);
}
void printcompleted()
{
		string eventname;
		eventname = "completedprint";
		wchar_t * t=new wchar_t[256];
        mbstowcs(t,eventname.c_str(),256);
		HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,t);
		SetEvent(printevent);
}
#elif linux
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
using namespace std;

#endif
#include <signals_events_setup.h>
#include <iostream>
#include <fstream>

char strings[10][30] = {{"1) First process"}, {"2) Second process"}, {"3) Third process"}, {"4) Fourth process"}, {"5) Fifth process"},
{"6) Sixth process"}, {"7) Seventh process"}, {"8) Eighth process"}, {"9) Ninth process"}, {"10) Tenth process"}};

int main(int argc, char *argv[])
{
#ifdef linux
    printSignal.sa_handler = canPrint;
    sigaction(SIGUSR1,&printSignal,NULL);
   printSignal.sa_handler = stopPrint;
   sigaction(SIGUSR2,&stopSignal,NULL);
#endif

    int number=1;
    number=atoi(argv[1]);
    while (true)
    {
#ifdef _WIN32
		int printFlag = waitforprint(argv[1]);		//под windows процесс должен сам периодически проверять, какие события произошли
		Sleep(100);									//в linux обработчик сигнала вызовется при получении сигнала
#endif
        if ( printFlag ==1)
        {
            for (int i = 0;strings[number-1][i]!=0;i++)
            {
                printf("%c",strings[number-1][i]);
                fflush(stdout);
                usleep(10000);
            }
            putchar('\n');
            fflush(stdout);
            printFlag=0;
            printcompleted();
        }
        if (printFlag== -1)
        {
			exit(0);
            break;
        }
    }
    return 0;
}

