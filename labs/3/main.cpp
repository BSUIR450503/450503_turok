
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


void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

char getch()
{
    char r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}
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
            //cout<<g;
            fflush(stdout);

            pr.receive();
            //pr.shmr[0]='b';
           // cout<<pr.shmr[0];
            //pr.receive();
        }
        g=mygetch();
        //if (g=='$') continue;
        if(g=='\r') putchar('\n');
        putchar(g);
        pr.send(g);
        pr.shm[0]=g;
        pr.shm[1]=0;
        //cout<<g;
        //g = getch();
        //cout<<123<<endl;
        /*if(g!='$')
        {
            pr.send(g);
        }*/

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
#ifdef linux
    key_t keyr=ftok("lab3",0);
    key_t key=ftok("lab3",1);
    int shmid = shmget(key, 100, 0666);
    shm = (char*)shmat(shmid, NULL, 0);
    int shmidr = shmget(keyr, 100, 0666);
    shmr = (char*)shmat(shmidr, NULL, 0);
#elif _WIN32
    HANDLE sendmap,receivemap;
    sendmap = ::CreateFileMappingW(
            NULL, // без файла на диске
            NULL, // не наследуется процессами-потомками
            PAGE_READWRITE,
            0, 100, // размер
            L"Local\\test-shared-memory2");
    receivemap = ::CreateFileMappingW(
                NULL, // без файла на диске
                NULL, // не наследуется процессами-потомками
                PAGE_READWRITE,
                0, 100, // размер
                L"Local\\test-shared-memory");
   shm = (char*)::MapViewOfFile(sendmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
   shmr = (char*)::MapViewOfFile(receivemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
#endif
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
        //fflush(stdout);
        if(g=='\r') putchar('\n');
        putchar(g);
        shm[0]=g;
        shm[1]=0;

    }
    exit(0);
}

