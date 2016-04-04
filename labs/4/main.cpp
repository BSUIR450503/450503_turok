#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "snprintf.h"
using namespace std;  


#elif linux
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <linux/sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
struct termios orig_termios;
#endif
#include "proc_object.h"
#include <iostream>
#include <cstdlib>
using namespace std;
#define MAX_COUNT 10
#ifdef linux

#endif



int main(int argc, char *argv[])
{
#ifdef linux
    non_block(NB_ENABLE);
#elif __WIN32
#endif
    int count=0;
    int current=1;
    myprocess * ms[10];
    int thread_numbers[10];
    string param;
    param.reserve(10);

    while (true)
    {

        char c = 'a';

        while(c!='q')
        {
          //c = getch();
          //c= (char)getchar_unlocked();
           c = mygetch();

          switch(c)
         {
            case '+':
          if(count < MAX_COUNT)
          {
            count++;
            //snprintf((char*)param.c_str(), sizeof(param.c_str()), "%d", count);
            thread_numbers[count]=count;
            myprocess * t = new myprocess(thread_numbers[count]);
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
         if(ms[current]->nextcanprint() && count>0)       // Если текущий процесс завершил печать
         {
           if(current > count)    // Если номер текущего процесса последний,
            current = 1;		       // то начинаем сначала
           ms[current-1]->allowprint();
           current++;

         }
              break;

        }

            usleep(100000); //чтобы не грузить процессор (10 миллисек.)
        }
            for (int i=0;i<count;i++)
            {

                ms[i]->endprint();

               }
        exit(0);

        }
    return 0;
    }

