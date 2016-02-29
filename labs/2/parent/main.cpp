#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <iostream>
#include "stdafx.h"
#include <cstdio>
#include <string.h>
#include "snprintf.h"
#define FNAME "lab2_child.exe"
using namespace std;  
#define flagEnd flagEndfunc(count)
int mygetch() 
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}
int flagEndfunc(int count)
{
	string eventname;
		eventname = "completedprint";
		wchar_t * t=new wchar_t[256];
        mbstowcs(t,eventname.c_str(),256);
		HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,t);
	if(WaitForSingleObject(printevent,100)== WAIT_OBJECT_0)
		{
			
			//cout<<"\n***STOP-START***"<<endl;
			if (!count) SetEvent(printevent);
			return 1;
		}
	return 0;
}
#elif linux
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <linux/sched.h>
#define FNAME "./lab2_child"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#endif
#include "proc_object.h"
#include <iostream>
#include <cstdlib>
using namespace std;
#define MAX_COUNT 10
#ifdef linux
int flagEnd = 1;
#endif

#ifdef linux
    void setEndFlag(int signum)
      {
      flagEnd = 1;
      }
#elif __WIN32
#endif


int main(int argc, char *argv[])
{

    setendsignal();
    int count=0;
    int current=1;
    myprocess * ms[10];
    string filename = FNAME;
    string param;
    param.reserve(10);
    int status;

    while (true)
    {

        char c = 'a';

        while(c!='q')
        {
          c = mygetch();
          switch(c)
         {
            case '+':
          if(count < MAX_COUNT)
          {
            count++;
            snprintf((char*)param.c_str(), sizeof(param.c_str()), "%d", count);
            myprocess * t = new myprocess(filename,param,0,status);
            ms[count-1]=t;
          };
              break;
          case '-':
          if(count > 0)
          {
            ms[count-1]->endprint(); // Завершаем последний дочерний процесс
            count--;
          };
              break;
         default:
         if(flagEnd && count>0)       // Если текущий процесс завершил печать
         {
           if(current > count)    // Если номер текущего процесса последний,
            current = 1;		       // то начинаем сначала
           ms[current-1]->allowprint();
           current++;

         }
              break;

        }

        }
            for (int i=0;i<count;i++)
            {

                ms[i]->killprocess();

               }
            exit(0);

        }
    return 0;
    }

