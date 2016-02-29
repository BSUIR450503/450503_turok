#ifndef SIGNALS_EVENTS_SETUP
#define SIGNALS_EVENTS_SETUP

#ifdef _WIN32

#elif linux
struct sigaction printSignal,stopSignal;
int printFlag = 0;
void canPrint(int signo)
{
  printFlag = 1;
}
void stopPrint(int signo)
{
  printFlag = -1;// остановить процесс
}
void printcompleted()
{
    kill(getppid(),SIGUSR2);
}
#endif

#endif // SIGNALS_EVENTS_SETUP

