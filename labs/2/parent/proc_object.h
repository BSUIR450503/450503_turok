#ifdef _WIN32
#include "stdafx.h"
#include "windows.h"
#include <string>
#include <iostream>
//#include <string.h>
#include <atlconv.h>
#define PID PROCESS_INFORMATION
/*using namespace std;
class myprocess
{
    public:
	PROCESS_INFORMATION proc;
    myprocess(string filename,string param,bool waitfor,int & status)
	{
        param=filename+' '+param;
		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &proc, sizeof(proc) );
		wchar_t * t=new wchar_t[256];
		mbstowcs(t,param.c_str(),256);
		if(!CreateProcess(NULL, t ,NULL,NULL,FALSE,0,NULL,NULL,&si,&proc))
			cout<<"FAIL"<<endl;
                if (waitfor)
			WaitForSingleObject(proc.hProcess,INFINITE);
            DWORD exit_code;
			GetExitCodeProcess(proc.hProcess, &exit_code);
            status=(int)exit_code;
	}
    myprocess(){};
	~myprocess()
	{
		CloseHandle(proc.hProcess);
	}
};*/
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
#define PID int
using namespace std;
#endif 

class myprocess
{
public:
    PID proc;
	string number;
#ifdef _WIN32
	HANDLE allowprintevent,stopprintevent;
    myprocess(string filename,string & param,bool waitfor,int & status)
    {
		number = param.c_str();
		//cout<<"number"<<number<<endl;
		//cout<<param.c_str()<<endl<<filename<<endl;
		param=filename + ' ' + param.c_str();
		//cout<<"params = "<<param<<endl;
        STARTUPINFO si;
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &proc, sizeof(proc) );
        wchar_t * t=new wchar_t[256];
        mbstowcs(t,param.c_str(),256);
		//wcout << t;
        if(!CreateProcess(NULL, t ,NULL,NULL,FALSE,0,NULL,NULL,&si,&proc))
            cout<<"FAIL"<<endl;
                if (waitfor)
            WaitForSingleObject(proc.hProcess,INFINITE);
            DWORD exit_code;
            GetExitCodeProcess(proc.hProcess, &exit_code);
            status=(int)exit_code;
		string eventname;
		eventname = number + "print";
		//cout<<"eventname:"<<eventname<<eventname.c_str()<<endl;
		//char evname[256];
		//snprintf(evname,sizeof("print12"),"%d%s",number,"print");
		//wchar_t * t=new wchar_t[256];
		mbstowcs(t,eventname.c_str(),128);
		allowprintevent = CreateEvent(NULL ,FALSE, FALSE, t );
		//wcout<<t;
		eventname = number + "noprint";
		//char eevname[256];
		//snprintf(eevname,sizeof("noprint12"),"%d%s",number,"noprint");
		wchar_t * tt=new wchar_t[256];
		//mbstowcs(tt,eevname,128);
        mbstowcs(tt,eventname.c_str(),128);
		//wcout<<tt;
		stopprintevent = CreateEvent(NULL ,FALSE, FALSE, tt );
    }

#elif linux
    myprocess(string filename,string param,bool waitfor,int & status)
    {
        int pipefd[2];
        pipe(pipefd);
             proc = fork ();
             if (proc == 0)
             {
               execlp(filename.c_str(),filename.c_str(),param.c_str(),"&",NULL);
             }
             else
           {
             if (waitfor)
                 wait(&status);
           }
          status = status / 256; //сдвигаемся влево, т к в linux только 8-16 биты содержат exit code
    }
#endif
    myprocess(){};
    void allowprint()
    {
		#ifdef linux
        kill(proc,SIGUSR1);
		#elif _WIN32
		//char printevent[3];
		//sprintf(printevent,"%s",number.c_str());
		
		SetEvent(allowprintevent);
		#endif
    }
    void endprint()
    {
		#ifdef linux
        kill(proc,SIGUSR2);
        waitpid(proc,NULL,NULL);
		#elif _WIN32
		ResetEvent(allowprintevent);
		SetEvent(stopprintevent);
		#endif
    }
    void killprocess()
    {
#ifdef linux
        kill(proc,SIGKILL);
        waitpid(proc,NULL,NULL);
#elif _WIN32
		DWORD c=0;
		TerminateProcess(proc.hProcess,c);
#endif
    }
};
#ifdef linux
void setEndFlag(int signum);
void setendsignal()
{
    struct sigaction endSignal;
    endSignal.sa_handler = setEndFlag;
    sigaction(SIGUSR2,&endSignal,NULL);



}
#elif _WIN32
	void setendsignal()
	{
		string eventname;
		eventname = "completedprint";
		wchar_t * t=new wchar_t[256];
        mbstowcs(t,eventname.c_str(),256);
		HANDLE completedprintevent = CreateEvent(NULL ,FALSE, TRUE, t );
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
        return getchar();
        reset_keypress(stored_settings);
    }
#endif
