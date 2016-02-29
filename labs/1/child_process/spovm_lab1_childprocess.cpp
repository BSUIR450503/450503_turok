// spovm_lab1_childprocess.cpp : Defines the entry point for the console application.
//
#ifdef _WIN32
#include "stdafx.h"
#elif linux
#include <cstdlib>
#endif
#include <iostream>
#include <fstream>
using namespace std;
int main(int argc, char * argv[])
{
    int x,opcode,y;
    
    x=atoi(argv[1]);
    x=x*x;
    ofstream file;
    file.open("tmp");
    file << x <<endl;
    file.close();
	return x;
}

