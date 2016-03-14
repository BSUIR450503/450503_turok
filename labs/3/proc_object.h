#ifdef _WIN32
//#include "stdafx.h"
#include "windows.h"
#include <string>
#include <iostream>
//#include <string.h>
//#include <atlconv.h>
#define PID PROCESS_INFORMATION

#elif linux
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PID int
using namespace std;
#endif 

class myprocess
{
public:
    PID proc;
	string number;
#ifdef _WIN32
    HANDLE sendmap,receivemap;
    char *shm, *shmr;
    myprocess(string filename,string & param,bool waitfor,int & status)
    {
        sendmap = ::CreateFileMappingW(
                NULL, // без файла на диске
                NULL, // не наследуется процессами-потомками
                PAGE_READWRITE,
                0, 100, // размер
                L"Local\\test-shared-memory");
        receivemap = ::CreateFileMappingW(
                    NULL, // без файла на диске
                    NULL, // не наследуется процессами-потомками
                    PAGE_READWRITE,
                    0, 100, // размер
                    L"Local\\test-shared-memory2");
       shm = (char*)::MapViewOfFile(sendmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
       shmr = (char*)::MapViewOfFile(receivemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		number = param.c_str();
        param=filename + ' ' + param.c_str();
        STARTUPINFO si;
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &proc, sizeof(proc) );
        wchar_t * t=new wchar_t[256];
        mbstowcs(t,param.c_str(),256);
        if(!CreateProcess(NULL, t ,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&proc))
            cout<<"FAIL"<<endl;
                if (waitfor)
            WaitForSingleObject(proc.hProcess,INFINITE);
            DWORD exit_code;
            GetExitCodeProcess(proc.hProcess, &exit_code);
            status=(int)exit_code;


    }

#elif linux
    char *shm;
    char *shmr;
    int pc[2];
    int cp[2];
    int shmid;
    int shmidr;
    myprocess(string filename,string param,bool waitfor,int & status)
    {
        pipe(pc);
        pipe(cp);
        close(cp[1]);
        //fcntl(pipefd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
             proc = fork ();
             if (proc == 0)
             {
                 usleep(1000);
               //execlp(filename.c_str(),filename.c_str(),param.c_str(),NULL);
                 execlp ("/usr/bin/xterm", "xterm", "-bd", "black", "-bg", "black",
                 "-fg", "green", "-geometry", "70x15+330+190", "-T", "lab3", "-e",
                 filename.c_str(),filename.c_str(),param.c_str(), (char *)0);
             }
             else
           {
             key_t key=ftok("lab3",0); //передача в child
             shmid = shmget(key, 100,IPC_CREAT | 0666);

             shm =(char*) shmat(shmid, NULL, 0);
             key_t keyr=ftok("lab3",1); //прием от child
             fflush(stdout);
             shmidr = shmget(keyr, 100,IPC_CREAT | 0666);
             shmr =(char*) shmat(shmidr, NULL, 0);
             if (waitfor)
                 wait(&status);
           }
          status = status / 256; //сдвигаемся влево, т к в linux только 8-16 биты содержат exit code
    }
#endif
    myprocess()
    {    }
    void send(char g)
    {
        shm[0]=g;
        shm[1]=0;
    }
    void receive()
    {
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

};
#ifdef linux

void term(int signum)
{
    TERMINATEFLAG=1;
}

    void setendsignal()
   {
        struct sigaction action;
        memset(&action, 0, sizeof(struct sigaction));
        action.sa_handler = term;
        sigaction(SIGTERM, &action, NULL);
    }
#elif _WIN32
	void setendsignal()
	{
        //string eventname;
        //eventname = "completedprint";
        //wchar_t * t=new wchar_t[256];
        //mbstowcs(t,eventname.c_str(),256);
        //HANDLE completedprintevent = CreateEvent(NULL ,FALSE, TRUE, t );
	}


#include <windows.h>
#include <stdio.h>
#include <conio.h>

HANDLE g_hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);

BOOL WINAPI HandlerRoutine(
  DWORD dwCtrlType   //  control signal type
)
{
  if (!g_hEvent)
    return FALSE;

  switch (dwCtrlType)
  {
    case CTRL_C_EVENT:
      SetEvent (g_hEvent);
      break;
    case CTRL_BREAK_EVENT:
      SetEvent (g_hEvent);
      break;
    case CTRL_CLOSE_EVENT:
      SetEvent (g_hEvent);
      break;
    case CTRL_LOGOFF_EVENT:
      SetEvent (g_hEvent);
      break;
    case CTRL_SHUTDOWN_EVENT:
      SetEvent (g_hEvent);
      break;
  }
  return TRUE; // as we handle the event
}
bool terminatecheck()
{
    if(WaitForSingleObject (g_hEvent, 10)==WAIT_OBJECT_0)
    {
    CloseHandle (g_hEvent);
    return true;
    }
    return false;
}


#endif
#ifdef linux
void set_keypress(termios & stored_settings)
    {
        struct termios new_settings;
        tcgetattr(0,&stored_settings);
        new_settings = stored_settings;
        /* для получения нажатых клавиш без потверждения enter-ом */
        new_settings.c_lflag &= (~ICANON); //перекл. в неканон. режим
        new_settings.c_lflag &= ~ECHO;     //не выводить нажатое
        new_settings.c_lflag &= ~ISIG;
        new_settings.c_cc[VTIME] = 0;      //ждем
        new_settings.c_cc[VMIN] = 1;       //считать хоть 1 символ
        tcsetattr(0,TCSANOW,&new_settings);
    }

    void reset_keypress(termios & stored_settings)
    {
        tcsetattr(0,TCSANOW,&stored_settings);
    }
    char mygetch()
    {
        struct termios stored_settings;
        set_keypress(stored_settings);
        char ret=getchar();
        reset_keypress(stored_settings);
        if (ret==EOF) return '$';
        return ret;

    }
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
    char * getshm()
    {
        char * shm;
#ifdef linux

    key_t key=ftok("lab3",1);
    int shmid = shmget(key, 100, 0666);
    shm = (char*)shmat(shmid, NULL, 0);
    return shm;
#elif _WIN32
    HANDLE sendmap;
    sendmap = ::CreateFileMappingW(
            NULL, // без файла на диске
            NULL, // не наследуется процессами-потомками
            PAGE_READWRITE,
            0, 100, // размер
            L"Local\\test-shared-memory2");
   shm = (char*)::MapViewOfFile(sendmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
   return shm;
#endif
    }
    char * getshmr()
    {
        char * shmr;
#ifdef linux
    key_t keyr=ftok("lab3",0);
    int shmidr = shmget(keyr, 100, 0666);
    shmr = (char*)shmat(shmidr, NULL, 0);
    return shmr;
#elif _WIN32
    HANDLE receivemap;
    receivemap = ::CreateFileMappingW(
                NULL, // без файла на диске
                NULL, // не наследуется процессами-потомками
                PAGE_READWRITE,
                0, 100, // размер
                L"Local\\test-shared-memory");
   shmr = (char*)::MapViewOfFile(receivemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
   return shmr;
#endif
    }
