#ifndef UTIL_H
#define UTIL_H
#include "windows.h"
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <QString>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;
char * getOsVersionString(DWORD major, DWORD minor, bool workstation);
void QueryKey(HKEY hKey, vector<string> &keyList);
#endif // UTIL_H
