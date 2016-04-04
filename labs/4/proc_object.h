#ifdef _WIN32
#include "stdafx.h"
#include "windows.h"
#include <string>
#include <iostream>
#elif linux
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
pthread_mutex_t * readymutex = new pthread_mutex_t();
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
using namespace std;
#endif 
const char strings[10][30] = {{"1) First process"}, {"2) Second process"}, {"3) Third process"}, {"4) Fourth process"}, {"5) Fifth process"},
{"6) Sixth process"}, {"7) Seventh process"}, {"8) Eighth process"}, {"9) Ninth process"}, {"10) Tenth process"}};
#define NB_ENABLE 1
#define NB_DISABLE 0
#ifdef linux
void * print_routine(void * n);
#elif _WIN32
void usleep(int s)
{
    Sleep(s/1000);
}
DWORD WINAPI print_routine(void * n);
CRITICAL_SECTION readymutex;
#endif
class myprocess
{

public:
    int this_thread_number;
#ifdef linux
    pthread_t * thread = new pthread_t();
    pthread_mutex_t * printmutex = new pthread_mutex_t();
    pthread_mutex_t * breakmutex = new pthread_mutex_t();
#endif
    int number;

#ifdef _WIN32
    HANDLE this_thread_handle;
    CRITICAL_SECTION printmutex,breakmutex;
    DWORD threadID;
    myprocess(int & thread_number)
    {
        this_thread_number = thread_number;
        InitializeCriticalSection(&printmutex);
        InitializeCriticalSection(&breakmutex);
        if (thread_number == 1)
        {
           InitializeCriticalSection(&readymutex);
        }
        this_thread_handle = CreateThread(NULL,100,print_routine,(void*)this,0,&threadID);
    }

#elif linux
    myprocess(int &thread_number)
    {
            this_thread_number = thread_number;

            pthread_mutex_init(printmutex,NULL);
            pthread_mutex_lock(printmutex);
            pthread_mutex_init(breakmutex,NULL);
            if (thread_number == 1)
            {
                pthread_mutex_init(readymutex,NULL);
            }
            pthread_create(thread,NULL,print_routine, (void*)this);
    }
#endif
    bool nextcanprint()
    {
#ifdef linux
        if (pthread_mutex_trylock(readymutex) != 0) return false;
        pthread_mutex_unlock(readymutex);
        return true;
#elif _WIN32
        if (TryEnterCriticalSection(&readymutex) == 0) return false;
        LeaveCriticalSection(&readymutex);

        return true;
#endif
    }


    void printcompleted()
    {
#ifdef linux
        pthread_mutex_lock(printmutex);
        pthread_mutex_unlock(readymutex);
#elif _WIN32
        LeaveCriticalSection(&printmutex);
        LeaveCriticalSection(&readymutex);
#endif
    }

    bool isBreak()
    {
#ifdef linux
        if (pthread_mutex_trylock(breakmutex) != 0) return true;
        pthread_mutex_unlock(breakmutex);
        return false;
#elif _WIN32
        if (TryEnterCriticalSection(&breakmutex) == 0) return true;
        LeaveCriticalSection(&breakmutex);
        return false;
#endif
    }

    bool isCompleted()
    {
#ifdef linux
        if (pthread_mutex_trylock(printmutex) != 0)
        {
            return true;
        }
        pthread_mutex_unlock(printmutex);
        return false;
#elif _WIN32
        if (TryEnterCriticalSection(&printmutex) == 0) return false;
        LeaveCriticalSection(&printmutex);
        return true;
#endif
    }
    myprocess(){}
    void allowprint()
    {
		#ifdef linux
        pthread_mutex_lock(readymutex);
        pthread_mutex_unlock(printmutex);
        #elif _WIN32
        EnterCriticalSection(&printmutex);
		#endif
    }
    void endprint()
    {
		#ifdef linux
        pthread_mutex_lock(breakmutex);
		#elif _WIN32
        EnterCriticalSection(&breakmutex);
        #endif
    }

};

#ifdef linux

    int kbhit()
    {
        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
        select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
        return FD_ISSET(STDIN_FILENO, &fds);
    }
    void non_block(int state)
    {
        struct termios ttystate;

           //get the terminal state
           tcgetattr(STDIN_FILENO, &ttystate);

           if (state==NB_ENABLE)
           {
               //turn off canonical mode
               ttystate.c_lflag &= ~ICANON;
               ttystate.c_lflag &= ~ECHO;
               //minimum of number input read.
               ttystate.c_cc[VMIN] = 1;
           }
           else if (state==NB_DISABLE)
           {
               //turn on canonical mode
               ttystate.c_lflag |= ICANON;
           }
           //set the terminal attributes.
           tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    }
    char mygetch()
    {
        char c;
        int i=kbhit();
                 if (i)
                 {
                     c=fgetc(stdin);
                     if (c=='q')
                         i=1;
                     else
                         i=0;
                 }
                 else c='z';
       return c;
    }
#elif _WIN32
int mygetch()
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}
#endif
#ifdef linux
    void * print_routine(void * n){
#elif _WIN32
    DWORD WINAPI print_routine(void * n) {
#endif
           myprocess * ptr = (myprocess*)n;
           while (true)
           {
               if ( !(ptr->isCompleted()) )
               {
#ifdef _WIN32
    EnterCriticalSection(&readymutex); //надо блокировать readymutex не из того потока где он проверяется
#endif
                   for (int i = 0;strings[(ptr->this_thread_number)-1][i]!=0;i++)
                   {
                       printf("%c",strings[(ptr->this_thread_number)-1][i]);
                       fflush(stdout);
                       usleep(50000);
                   }
                   putchar('\n');
                   fflush(stdout);
                   ptr->printcompleted();
               }
               else
               usleep(10000);
               if (ptr->isBreak())
               {
                   break;
               }
           }
    }
