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
