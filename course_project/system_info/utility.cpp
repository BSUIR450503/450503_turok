#include "utility.h"

char * getOsVersionString(DWORD major,DWORD minor,bool workstation=true) {
    switch(major) {
    case 10: return " 10";
    case 6: switch (minor) {
        case 3: {
            if (workstation) return " 8.1";
            return " Server 2012 R2";
        }
        case 2: {
            if (workstation) return " 8";
            return " Server 2012";
        }
        case 1: {
           if (workstation) return " 7";
           return " Server 2008 R2";
        }
        case 0: {
            if (workstation) return " Vista";
            return " Server 2008";
        }

    }
   case 5: switch (minor) {
        case 2: {
            return " Server 2003";
        }
        case 1: {
            return " XP";
        }
        case 0: {
            return " 2000";
        }
        }
    }
        return "unknown";
}

void QueryKey(HKEY hKey, vector<string> & keyList)
{
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name
    DWORD    cchClassName = MAX_PATH;  // size of class string
    DWORD    cSubKeys=0;               // number of subkeys
    DWORD    cbMaxSubKey;              // longest subkey size
    DWORD    cchMaxClass;              // longest class string
    DWORD    cValues;              // number of values for key
    DWORD    cchMaxValue;          // longest value name
    DWORD    cbMaxValueData;       // longest value data
    DWORD    cbSecurityDescriptor; // size of security descriptor
    FILETIME ftLastWriteTime;      // last write time

    DWORD i, retCode;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count.
    retCode = RegQueryInfoKey(
        hKey,                    // key handle
        achClass,                // buffer for class name
        &cchClassName,           // size of class string
        NULL,                    // reserved
        &cSubKeys,               // number of subkeys
        &cbMaxSubKey,            // longest subkey size
        &cchMaxClass,            // longest class string
        &cValues,                // number of values for this key
        &cchMaxValue,            // longest value name
        &cbMaxValueData,         // longest value data
        &cbSecurityDescriptor,   // security descriptor
        &ftLastWriteTime);       // last write time

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (cSubKeys)
    {
        for (i=0; i<cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey,
                     &cbName,
                     NULL,
                     NULL,
                     NULL,
                     &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS)
            {
                QString s = QString::fromWCharArray(achKey);
                keyList.push_back(s.toStdString());
            }
        }
    }
}
