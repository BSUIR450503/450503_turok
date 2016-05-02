#include "manager.h"
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    int * ms = (int*)malloc(100 * sizeof(int));
    for (int i=0;i<100;i++){
        ms[i]=i;
    }
    //free(ms);
    for (int i=0;i<10;i++){
        cout<<ms[i]<<endl;
    }
    int * mss = (int*)malloc(10 * sizeof(int));
    for (int i=0;i<10;i++){
        mss[i]=i*2;
        cout<<mss[i]<<endl;
    }
    mss = (int*)realloc(mss,15 * sizeof(int));
    for (int i=0;i<15;i++){
        cout<<mss[i]<<endl;
    }
    for (int i=0;i<10;i++){
        cout<<ms[i]<<endl;
    }
    free (ms);
    ms = (int*)malloc(100 * sizeof(int));
    for (int i=0;i<10;i++){
        cout<<ms[i]<<endl;
    }

    return 0;
}

