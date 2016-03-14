
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <iostream>
//#include "stdafx.h"
#include <cstdio>
#include <string.h>
//#include "snprintf.h"
#define FNAME "lab3.exe"
#define TERMINATEFLAG terminatecheck()
using namespace std;

int mygetch()
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}
void usleep(int a)
{
    Sleep(a/1000);
}
#elif linux
volatile int TERMINATEFLAG;
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <linux/sched.h>
#define FNAME "./lab3"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
struct termios orig_termios;



#endif
#include "proc_object.h"
void childprogram();
int main(int argc, char *argv[])
{
#ifdef linux
 set_conio_terminal_mode();
#elif _WIN32
    SetConsoleCtrlHandler (HandlerRoutine, TRUE);
#endif
 setendsignal();
        for (int i=0;i<argc;i++)
        {
        if(!strcmp(argv[i],"child")) childprogram();
        }
        cout<<"not child"<<endl;
        fflush(stdout);
    char g='$';
    string param,filename;
    filename = FNAME;
    param = "child";
    int result;
    myprocess pr(filename,param,0,result);

    pr.shmr[0]=0;
    pr.shmr[2]=0;
    while(true)
    {
        while (!kbhit())
        {
            if(TERMINATEFLAG)
            {
                pr.shm[2]='T';
                exit(0);
            }
            usleep(10000);
            g='$';
            fflush(stdout);

            pr.receive();
        }
        g=mygetch();
        if(g=='\r') putchar('\n');
        putchar(g);
        pr.send(g);
        pr.shm[0]=g;
        pr.shm[1]=0;

        fflush(stdout);
    }
    return 0;
}
void childprogram()
{
    myprocess cp;
    char g='$';
    cout<<"child"<<endl;
    char * shm, *shmr;
    shm=getshm();
    shmr=getshmr();
    shmr[0]=0;
    shmr[2]=0;
    while(true)
    {
        while (!kbhit())
        {
            if(TERMINATEFLAG)
            {
                shm[2]='T';
                exit(0);
            }
            usleep(10000);
            fflush(stdout);
            if(shmr[2]=='T') exit(0);
            if(shmr[0])
            {
                if(shmr[0]=='\r')
                   cout<<'\n';

                putchar(shmr[0]);
                shmr[0]=0;
                fflush(stdout);
            }
        }
        g=mygetch();
        if(g=='\r') putchar('\n');
        putchar(g);
        shm[0]=g;
        shm[1]=0;

    }
    exit(0);
}

