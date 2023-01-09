#include <cstdio>
#include <cstdlib>
#include <vector>
#include <errno.h>
#include <string.h>

#include "scope_optimizer.h"
char* loadFile(const char* fileName);

int main(int argc, char** argv)
{
    std::string output, localsScope;
    char* input = loadFile("../test.h");

    ::OptimizeScope(input, output, 41, localsScope);

    printf("Locals: %s\n", localsScope.c_str());
    printf("All   : %s\n", output.c_str());
    return 0;
}

//-------------------------------------------------------
// Help function
//-------------------------------------------------------
char* loadFile(const char* fileName)
{
    FILE* fp;
    long len;
    char* buf = NULL;

    fp = fopen(fileName, "rb");
    if(!fp) {
        printf("failed to open file 'test.h': %s\n", strerror(errno));
        return NULL;
    }

    // read the whole file
    fseek(fp, 0, SEEK_END); // go to end
    len = ftell(fp); // get position at end (length)
    fseek(fp, 0, SEEK_SET); // go to beginning
    buf = (char*)malloc(len + 1); // malloc buffer

    // read into buffer
    long bytes = fread(buf, sizeof(char), len, fp);
    printf("read: %ld\n", bytes);
    if(bytes != len) {
        fclose(fp);
        printf("failed to read from file 'test.h': %s\n", strerror(errno));
		free(buf);
        return NULL;
    }

    buf[len] = 0; // make it null terminated string
    fclose(fp);
    return buf;
}
