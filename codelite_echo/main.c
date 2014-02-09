#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int i=1;
    for(; i<argc; ++i) {
        if ( strstr(argv[i], " ") || strstr(argv[i], "\t") ) {
            // escape with double quoutes
            printf("\"%s\" ", argv[i]);
        } else {
            // print it as it is
            printf("%s ", argv[i]);
        }
    }
    printf("\n");
    return 0;
}
