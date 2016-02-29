#ifdef _WIN32
#include "stdafx.h"
#include "windows.h"
#include <string>
#include <iostream>
#include <atlconv.h>
using namespace std;
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
	~myprocess()
	{
		CloseHandle(proc.hProcess);
	}
};
#elif linux
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
class myprocess
{
public:
    myprocess(string filename,string param,bool waitfor,int & status)
    {
        int pid;
             pid = fork ();
             if (pid == 0)
             {
               execlp(filename.c_str(),filename.c_str(),param.c_str(),NULL);
             }
             else
           {
             if (waitfor)
                 wait(&status);
           }
          status = status / 256; 
    }
};
#endif
