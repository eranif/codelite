#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#define SECONDS_TO_SLEEP 15

int main(int argc, char** argv)
{
    int i = 1;
    for(; i < argc; ++i) {
        if(strstr(argv[i], " ") || strstr(argv[i], "\t")) {
            // escape with double quoutes
            printf("\"%s\" ", argv[i]);
        } else {
            // print it as it is
            printf("%s ", argv[i]);
        }
    }
    printf("\n");
#ifdef WIN32
    Sleep(SECONDS_TO_SLEEP * 1000); // Under Windows its in ms
#else
    sleep(SECONDS_TO_SLEEP); // sleep for 100 seconds
#endif
    return 0;
}
